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

For reference, my host machine is running Fedora 36 and I use Boxes to run the VM. It doesn't matter what you use to virtualise, as long as you can select "Fedora server 36" as the VM.

Before installation, make sure to give your VM as many CPU cores as you can. 100GB ought to be more than enough disk space. Whilst installing your F36 server VM, make sure to enable root and to create an administrator account. 

The reason for using F36 server edition is there is no GUI for you to use, forcing you poke around in the terminal. :D

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
`$ sudo grub-set-default 0`

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



























