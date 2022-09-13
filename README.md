# kernel_workshop
A workshop for new Red Hat interns (and others) as an introduction to building and debugging the Linux Kernel, as well as making some changes to it along the way.

Compiled by Jonathan Cameron, jcameron@redhat.com

## Workshop Goals
- Give students an understanding of how to build, install, and debug your own Linux kernel.
- Demonstrate changes to the kernel, including adding a new syscall, new variables to the /proc filesystem, and how to load a kernel module into the system.
- Encourage students to become accustomed to using the terminal and its range of tools.
- Teach students to work their way through problems, and to not shy away from Google.

## Workshop Description
Students will go through the steps to:
1. Build their own kernel
2. Build and deploy a simple kernel module
3. Start the kernel in a debugger
4. Patch the kernel
5. Define and add a new syscall
6. Add page-cache hit/miss counters to the Linux kernel
7. Export the counters to the /proc filesystem
8. Implement a user program that exercises the page-cache using file IO
9. Implement a user program that monitors and displays page-cache hit/miss ratios.

Going forwards,
- // is to signify a comment about why we're doing what we're doing
- $ is an instruction you will give your machine

## Configure your Virtual Machine (VM)
Due to the nature of this exercise, I would recommend using a VM, although you are welcome to do this on bare-metal (your own machine). There are a few benefits to using a VM.
- If you brick the VM, you won't destroy your own machine.
- You can follow more closely using the same machine I used for this exercise.

For reference, my host machine is running Fedora 36 and I use Boxes to run the VM. It doesn't matter what you use to virtualise, as long as you can select "Fedora 36" as the VM.

Before installation, make sure to give your VM as many CPU cores as you can. 100GB ought to be more than enough disk space. Whilst installing your F36 VM, make sure to enable root and to create an administrator account if that is an option. 

## Update the kernel and download some useful packages
Chances are you will have installed the latest and greatest if you downloaded your kernel image, but just to be safe.

// Update and reboot your VM
`$ sudo dnf update`
`$ sudo init 6`

// Install some useful packages we'll need later for building and editing our own kernel.
`$ sudo dnf groupinstall "C Development Tools and Libraries"`
`$ sudo dnf install elfutils-devel kernel-devel git openssl-devel ncurses-devel gcc make ctags bison flex elfutils-libelf-devel bc wget perl dwarves openssl`

// Check the current kernel we're running and reboot
`$ uname -srm`
`$ sudo init 6`

## Build a vanilla upstream kernel
Building an upstream kernel is easy. Simply download the code, configure the kernel, compile the kernel, install it, then reboot the VM and MAKE SURE YOU BOOT THE NEW KERNEL

### Cloning (downloading) the upstream kernel tree from GitHub.com
// We use --depth=1 because the Linux source is a very large repository and we don't want waste time and space downloading code that we don't need. This will pull the latest commit from the main branch.
`$ git clone --depth=1 https://github.com/torvalds/linux.git`
// See the new directory, "linux"
`$ ls -al`

// Change into the linux directory
`$ cd linux`

// Make a build directory to put config files in later
`$ mkdir build

// Clean up any stale content that may be left over in the linux repo
`$ make mrproper && make clean`

Assuming the current running kernel os older than the kernel you're about to build (updating the kernel to a newer upstream version/release), the quickest way to configure the new kernel is to base the config options options on the config of the running kernel. On Fedora hosts, one can check the configuration of the current running kernel by accessing the abailable /boot/config-$(uname -r) file. 

The Linux kernel build infrastructure expects the config options to be declared within a .config file in the source directory, therefore to get the vanilla istream kernel set up as a generic Fedora kernel, we need to use the following commands:

// Copy the config of the current system as a basis
`$ sudo cp /boot/config-$(uname -r) build/.config`

// Use the current config with the new kernel
`$ make O=build olddefconfig`

(Note: The second step is required in order to update the current config file, using the provided .config while setting any new config symbols to its default valie without prompting questions about setting up these symbols.)

If you want further customisation is wanted/required, you can resort to an ncurses interface offered by the kernel config infrastructere to make the set up process easier.

// Optionally run menuconfig
`$ make O=build menuconfig`

Generic distro kernels (e.g., Fedora's) commonly have hundreds or thousands of kernel components configured as modules. This is useful to ensure that the resulting kernel and modules can run on the largest variery of computer systems. However, more modules implies longer kernel compile times (i.e., a default config will easily take hours to compile). Given that we know exactly which machine the new kernel will run on (the VM that we're currently operating in), we can speed up the compile time by only building the modules we actually need.

// Update config to only build the necessary modules. If you miss this step, the next steps will take hours. You might get some options to answer when you run this command. Press yes.
`$ make O=build localmodconfig`

## Building the kernel
Now it is time to actually compile the kernel. It can take a long time but we can run this task on different cores simultaneously and get it done faster. To find out the number of cores your machine has, use `nproc`. Depending on your hardware, you'll want to use a multiple of n parallel processes to compile the kernel. Some machines run on slow spinning rust hard drives. Hence the compiler will oftern wait for IO to read source files from the disk and write the object files back. Choosing the right number of cores is tricky, but with spinning rust HDDs choosing n = 2*#cores is a decent choice.

To compile the kernel, use the following command

// Build the kernel and modules. This will take a while.
`$ time make O=build -j``nproc`` bzImage modules` 

This step will take some time. If you configured the kernel and modules correctly, as described above, it should take about 10-15 minutes on a Red Hat machine.

## Installing the new kernel on your machine
Imagine you made some really cool changes to the kernel that you want to deploy on the same machine you are working on. You can install the new kernel on your machine in the following way:

(Note: For the next few steps, you _must_ be root or run via sudo.)

// Install the modules and kernel we just built.
`$ sudo make O=build modules_install install`

// Set the new kernel as the default to boot into.
`$ sudo grub2-set-default 0`

// Check what we're currently running
`$ uname -srm`

// Reboot. Pay attention at the grub menu to choose our new kernel. (It shouldn't match the string printed by the command above, unless your VM was already running a clone from the linux repo)
`$ sudo init 6`

// Login and check what kernel version is running.
`$ uname -srm`

(Note 1: Replaying the steps above without changin the "EXTRAVERSION" string will caise the kernel image and modules to be overwritten by the recently compiled images. If overwriting the kernle artifacts is not the intention, the EXTRAVERSION string must be incremented for every build/install.)

(Note 2: These are the basic steps to build an upsteam kernel and install it on a Fedora 36 sustem . From here, kernel development labs can be developed.)

## Change the EXTRAVERSION string in the kernel Makefile.

Let's make a small change to the kernel, recompile, and observe that we're running a newly compiled kernel. An easy thing to change is the EXTRAVERSION string of the kernel.

// Go to our linux directory.
`$ cd linux`

// Open the Makefile with your favourite text editor. I will upset some people by using Vim. This is because I haven't learnt Emacs... yet.
`$ vim Makefile`

// Edit the EXTRAVERSION string. Maybe append .0 to it. Add your name to the name to the name field as well! Then it's yours :) Save the changes and exit.
 
// Rebuild the kernel and modules
`$ time make O=build -j``nproc`` bzImage modules

// Install
`$ sudo make O=build modules_install install

// Reboot, make sure to boot the right kernel.
`$ sudo init 6`

// Check the version. If you booted the right kernel, you should see a change from the last kernel you booted!
`$ uname -srm`

## Building (out of the tree) kernel modules
It's usually not necessary to download and rebuild the entire kernel if the objective is just to (re)build a module. To build a kernel module for the currently running kernel, only the C development tools, elfutil-devel, and the matching kernel-devel RPMs are required. (They are already installed in the VM.)

## These are the bare-bones to start with building out of tree kernel modules:

// Move back to the home directory.
`$ cd`

// Clone a kernel module directory.
`$ git clone https://github.com/megele/dummy-kmod

// Move into this repo.
`$ cd dummy-kmod`

// Quickly change the name of the .c file in the directory
`$ mv dummy.c hello.c`

// Compile the module.
`$ make`

// Load the kernel module
`$ sudo insmod hello.ko`

//Inspect the effect of loading the module on one of the internal message boards.
`$ dmesg`

// Unload the kernel module
`$ sudo rmmod hello.ko`

// Inspect the effect of unloading the module.
`$ dmesg`

// Here would be a good time for you to edit the module, just so you go in and take a look at the code.
`$ vim hello.c`

// Make and load again
`$ make && sudo insmod kello.ko && dmesg`

// Unload
`$ sudo rmmod hello.ko`

(Note: Now would be a good time to google kernel tainting, KERNEL_TAINT flags and their meaning, as well as signed modules and their importance. Signing the module binary code can be explored later.)

### Suggestions to expand the skeleton module, as bonus exercises:
1. Extend the module to take different strings, to show on the console, from the module parameters list, using the module_param() macro.
2. Export kernel internal constants, like HZ and USER_HZ to userspace via:
	a. module parameters, which will automatically be exported to sysfs interface:
		/sys/module/<modname>/parameters/<params>
	b. procfs entries

(Note: as soon as students realise the power of extending the kernel with out-of-tree modules, almost any task can be accomplished via this approach. Further reading on the topic: Corbet et al. Linux Device Drivers, Third Edition: https://lwn.net/Kernel/LDD3/ )

## Running the new kernel with QEMU.
QEMU is a allows us to boot an OS and then debug it, which we can do now that we have compiled our unique Linux OS. To boot it in emulation, we need something called an initramfs or initrd which is essentially a very small, memory based file system (a quick Google here could be helpful). It contains all the libraries and code required to boot up the kernel. Once the kernel boots up, it can load the actual file system and do normal operations. We use a tool called supermin to create this initrd.

### Preparing to run the kernel
To install supermin, use the following command:
`$ sudo dnf install supermin` 

Supermin works in two stages, prepare and build. Prepare is where we specify waht packages and libraries to include in the initrd. Build is where this initrd actually gets made. Here are simple commands we can use to prepare a simple initrd.

//Leave the kernel directory and go up one level.
`$ cd ..`

// Make initrd and rootfs.
`$ mkdir initrd && cd initrd`

// Prepare the supermin.d subdirectory. This might hang a little while.
`$ supermin --prepare bash coreutils -o supermin.d`

// Write a script that welcomes and starts bash.
`$ echo -e '#!/bin/bash\necho Welcome\nexec bash' > init`

// Change the scripts permissions so we can execute it.
`$ chmod 0755 init`

// Package our init file into a zip file.
`$ tar zcf supermin.d/init.tar.gz init`

// Build appliance.d 
`$ supermin --build --format ext2 supermin.d -o appliance.d`

// Take a look at what we have done with the past 6 instructions.
`$ tree`

## Running the kernel with QEMU
Now that we have a kernel and an initrd ready, we can now boot these up. There are many ways we can do this, we will choose QEMU which is a virtual machine emulator. 

Now let's go up one directory to actually run everything. This step is not required but it'll be easier to understand the relative paths of the kernel and initrd from this main directory when we boot up our kernel in the next step.

Assuming you are running x86_64 architecture. the QEMU command you will use is "qemu-system-x86_64". This command takes a bunch of options telling it where the kernel is, where the initrd is and so on. Below is agerneric command which will run your kernel and show the output in the same terminal window. You can read up the qemu man pages and change the options in you want.

GENERIC COMMAND:
	qemu-system-x86_64 -nodefaults -nographic -kernel <path/to/kernel_image> -initrd <path/to/initrd? -hda <path/to/root_disk? -serial stdio -append "console=ttyS0 root=/dev/sda nokaslr"

In the command above we need to provide path to the kernel image, intid and the root disk.

The kernel image that we need to on our x86 machine would be at a specific place in our Linux directory. The path in this example is /linux/build/arch/x86/boot/bzImage. For initrd, the path would be /initrd/appliance.d/initrd. And the root disk will be at this location initrd/appliance.d/root.

// This makes a bit more sense when we look at tree initrd from the home directory.
`$ tree initrd`

Then, pay attention to the location of the bzImage. It's in /linux, the kernel we cloned. It's in /linux/build, the directory we created to build our own kernel in. Then it's in /linux/build/arch/x86. This makes sense, if you are running an x86 machine. We only built we our own machine. Finally, it's in /linux/build/arch/x86/boot/bzImage, which is how our VM boots the kernel we want. Just a small aside to hopefully explain things.

The final commmand would look something like this:

// Start QEMU
`$ qemu-system-x86_64 -nodefaults -nographic -kernel linux/build/arch/x86/boot/bzImage -initrd initrd/appliance.d/initrd -hda initrd/appliance.d/root -serial stdio -append "console=ttyS0 root=dev/sda nokaslr"`

Sit back and observe QEMU boot your new kernel. You should play about (write to some files in the home directory) with the filesystem whilst I find an answer about why memory is persistent when creating files.

If you get an error regarding undefined libusb symbols, this means your libusb package needs to be updated. You need libusb-1.0.22-1.fc28 version, anything older than this will not work. You can download and install the latest rpm (Red Hat Package Manager) yourself through the following commands.

// Update libusb
`$ wget https://dl.fedoraproject.org/pub/fedora/linux/updates/28/Everything/x86_64/Packages/l/libusbx-1.0.22-1.fc28.x86_64.rpm`
`$ rpm -U libusbx-1.0.22-1.fc28.x86_64.rpm`

Run the qemu command again and you will see your kernel booting up.

// Exit QEMU
`$ <crtl-c>`

## Debugging the kernel with GDB
Once you make modifications to the kernel, you might want to debug it in case something is not working.

### Changing the QEMU command
QEMU gives a very nice interface for debugging with GDB. When running the kernel with QEMU, add two options to the command i.e., "-s -S". These two options will make sure that you can step through the kernel code.

The final QEMU command will be as follows.

// Start QEMU in debug mode (add -s -S to the above command)
`$ qemu-system-x86_64 -s -S -nodefaults -nographic -kernel linux/build/arch/x86/boot/bzImage -initrd initrd/appliance.d/initrd -hda initrd/appliance.d/root -serial stdio -append "console=ttyS0 root=/dev/sda nokaslr"`

The -S flag stops the CPU from starting up. Another instruction needs to be sent to QEMU to make it start. If we type:
`$ man qemu`, the manual will tell us a little bit about it.

The -s flag opens a GDB server on TCP port 1234. This is the interface we use to continue execution of instructions. Google "debugging with QEMU" to get some help.

### Starting GDB
Now if you're using Boxes, this next bit will be almost impossible unless you can figure out a way to SSH into the server, because of how Boxes handles the network bridges into the VM. If you can figure out an easy way, please send me an email.

In a separate terminal window, open GDB by typing gdb as follows.

// SSH into the VM with another terminal. Then type:
`$ gdb ~linux/build/vmlinux

(gdb) `$ target remote localhost:1234`
0x000000000000fff0 in exception_stacks ()

(gdb) `$ x/3i $rip`

From here, you can start to dig around in the kernel as it boots up, using GDB to step through instructions.

## Stop the kernel and find the corresponding source
While booting up the kernel in QEMU with GDB attached, stop the kernel's boot process (press ctrl-c in GDB). Then GDB will show which function the kernel is currently in.

For example, after booting the kernel in debug mode with the QEMU command above, and logging into another terminal to run GDB, continue the kernel's boot process.

// Type "c" into GDB which is listening on localhost:1234
(gdb) `$ c`
Continuing.
`ctrl-c`
Program received signal SIGINT, Interrupt.
0xffffffff813f2f8e in ext4_mpage_readpages
.....

The question is, how can we find the function above, ext4_mpages_readpages(), in the 2 million lines of kernel source?

We could "grep" the entire source, which is an awfully slow and inefficient way to navigate source code. So if grep is not right, what can we do? Cscope comes to our rescue. With Cscope, we can build an index of the source tree once and then reuse that index to quickly find/jump to any symbol we want. 

// Install Cscope
`$ sudo dnf install cscope`

// Navigate to the Linux source directory
`$ cd linux`

// Run cscope to build the index (-b), recursively over all the files in the same directory (-R)
`$ cscope -R -b -v`

// Use vim to open whichever C file has the implementation of the function we're looking for.
`$ vim -t ext4_mpage_readpages`

// Exit vim once you've had a look around the code.
`:q`

## Kernel Patching
In the Linux kernel, changes to the source code are implemented via kernel patches. THese kernel patches are unified diffs of the source repository and can be seen by the output of the following commands

// Enter Linux directory
`$ cd linux`

// See the recent commits. Since we cloned with --depth=1, we only see one recent commit.
`$ git log`

### Creating a kernel patch
This next part of the tutorial will be really helpful in the future for when you start working with large codebases with multiple contributors. It will also be a bit like school though... You probably won't use this stuff exactly, but it'll be nice to be familiar...

// See what is changed in your Linux kernel since we cloned from the repo.
`$ git diff` 

Let's start a new branch with Git.
Git is a fantastic tool for version control. It accurately keeps track of who added what to the codebase and when it was added. And if the additions are no good, they can easily be removed. By using our own branch to develop our own kernel, we don't have to worry about new releases. Also, we don't have to worry about corrupting the main branch with our developments if we work in our own branch. We can always merge our changes to the main branch later with a pull request.

// Create and switch to our own new branch
`$ git checkout -b eddybox`

// Check that we're in another branch
`$ git branch`

We can use git commit and git format-patch to create a series of patches that others could then load into their own kernels, or we could merge these patches to the upstream kernel.

// Commit the changes to our branch. At the moment this should just be our edits to the Makefile. Follow the instructions on screen when Git asks you to tell it who you are. A text editor will open, and this is where you should write a commit message to let others know what changed since the last commit. Something like "Our first edit, changed the Makefile."
`$ git commit -a`

// Check the commit log
`$ git log`

// Prepare a patch from the latest commit. This creates/formats 1 patch from the HEAD of the git log. The most recent commit is the one we just made, and we can see this when we read the header of our recent commit with "git log". So when we break the next command down, we are telling git to make us a patch that includes every change made from the HEAD to -1 from the HEAD. Which just means putting our Makefile changes in a patch.
`$ git format-patch -1 HEAD`
> 0001-<The commit message you entered>.patch

Now we can apply and remove this patch to the kernel.

### Applying and removing a patch to the unchanged kernel.
Let's start by checking the Makefile.

// Print the top few lines of the Makefile to the screen. We will see our edits to the EXTRAVERSION and NAME.
`$ cat Makefile | less`

Since the patch is already applied (we did it manually), we can remove it with the following. 

// Install patch.
`$ sudo dnf install patch`

// Remove the most recent patch.
`$ patch -p1 -R < 0001-Our-first-edit!.patch`

// Take a look at the Makefile and see our changes have gone!
`$ cat Makefile | less`

// Reapply the patch
`$ patch -p1 < 0001-Our-first-edit.patch`

Of course, we could also use git to revert changes, but this way, we have a file we can send to other people via email. If you really mess things up or want to back things up for whatever reason, we can reset the sources back to how we cloned them

// DON'T DO THIS UNLESS YOU ACTUALLY WANT TO GO BACK TO HOW YOU CLONED THE SOURCE.
`$ git reset --hard <Commit ID>

For example:

// Git diff will show your changes
`$ git diff`
	<some changes>

// Git log will show all of the commits since you cloned the project. If you used --depth=1, you will only see a git log to that point.
`$ git log`
	commit 3e45cb03..... (HEAD -> eddybox)
	Author: Jonathan <jcameron@redhat.com>
	Date: 	Tue Sep 13.....

		Our first edit!

	commit 50635787..... (grafted, origin/master, origin/HEAD, master)
	Author: Linus Torvalds <torvalds.....
	Date:	Thu Sep 8.....

		Merge tag 'spi-fix-.....
		.....

//So then to undo our changes, we can use:
`$ git reset --hard 50635787.....`

// Check the logs
`$ git diff`
	<no changes>

Now we will learn how to send all the patches that we're going to make to the upstream community.

// Install git-email.
`$ sudo dnf install git-email`

Because we are using git-email for the first time, we will need to configure the smtp (ongoing mail) server settings using the "git config" utility. Type the following commands in the terminal to send from Red Hat's smtp server using your Red Hat email address. If you don't have a Red Hat email account, you'll need to find the setting for your account.

1. git config --global sendemail.smtpuser <Your Red Hat email>
2. git config --global sendemail.smtpencryption ssl
3. git config --global sendemail.smtpserver smtp.gmail.com
4. git config --global sendemail.smtpserverport 465

You can check if the configuratio is what you wanted in the fields we just edited with the following two methods.

// Retype the command without the value.
`$ sudo git config --global sendemail.<key>

OR

// View the /.gitconfig file.
`$ cat /home/fedora/.gitconfig

Now we're going to use the utility to send a patch to ourselves, just to check everything is working. This is part of the tutorial where YMMV. Send-email is tricky to get working with 2FA.

// Initiate the utility and follow the prompts.
`$ git send-email --suppress-cc=all 0001-Our-first-edit.patch --from=<Your email>`

## Defining a new System Call and creating a patch to distribute it.
Depending on the complexity of the syscall, it can be very easy or a bit tricky to add a one, depending on what code paths the function touches.

To do it, we need to carry out three steps
1. Add the new syscall to the end of the syscall table in arch/x86/entry/syscalls/syscall_64.tbl, taking the next available system call number.
2. Add the appropriate asmlinkage declaration in include/linux/syscalls.h
3. Add your new syscall code in kernel/sys.c using the SYSCALL_DEFINE2(...) macro. (The macros will do most of the work for you "automagically".

1. Editing the syscall table

// Change into the linux working directory, make sure you are in your branch.
`$ cd linux`

// Open the syscall table file. Maybe take some time to poke around in the source code in this directory. You can add a syscall wherever you like in this table, as long as it doesn't clash with the comments already in the file.
`$ vim arch/x86/entry/syscalls/syscall_64.tbl`

Here you should enter (I chose 451 for this example):
	451	common	add2int		sys_add2int
Save and quit.

2. Editing the syscall header file.

// Open the header file
`$ include/linux/syscalls.h`

Here you can make edits almost anywhere, but I would recommend doing it after all of the headers, includes and DEFINEs. If you can find a gap between some other "asmlinkage long..." prototypes that would be good to keep things neat and tidy.

Add a comment that you can search to go back to easily.
	/* Kernel workshop additions go here. */
	asmlinkage long sys_add2int(long a, long b);
Save and quit.

3. Edit the syscall file.

// Open the syscall file.
`$ vim kernel/sys.c`

Again, find a suitable place amongst the other function definitions and add this block.
	/* This is the definition of our new function that adds two integers. */
	SYSCALL_DEFINE2(add2int, long, a, long, b)
	{
		long result = a + b;
		printk(KERN_INFO "syscall: add2int: a = %ld, b = %ld, result = %ld\n", a, b, result);
		return result;
	}
Save and quit.

### Rebuilding the kernel to add the new system call.
// Rebuild the kernel
`$ time make O=build -j``nproc`` bzImage modules`

// Install the new kernel
'$ sudo make O=build modules_install install`

// Set the new default to boot into.
`$ sudo grub2-set-default 0`

// Reboot
`$ sudo init 6`

Congratulations! You have just added a new syscall to your own version of the Linux kernel! How do you feel? Let's test the function!

// Go to /linux, make a directory for testing, and open a new file.
`$ vim test.c

Write a C program to test your syscall:
	/*
	 * Test the add2int syscall (451)
	 */
	#define _GNU_SOURCE
	#include <unistd.h>
	#include <stdlib.h>
	#include <sys/syscall.h>
	#include <stdio.h>

	#define SYS_add2int 451

	int main(int argc, char *argv)
	{
		int a, b;
		if (argc <= 2) {
			printf("Must provide 2 integers to pass to the system call\n");
			return -1;
		}
		a = atoi(argv[1]);
		b = atoi(argv[2]);

		printf("Making a system call with a = %ld, b = %ld\n", a, b);
		long res = syscall(SYS_add2int, a, b);
		printf("System call returned %ld\n", res);

		return 0;
	}

// Compile the program.
`$ gcc test.c -o test`

// Test it. It's called _add_ 2int for a reason ;)
`$ ./test 4 5`




	





































