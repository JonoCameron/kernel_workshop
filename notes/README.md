# Menno's class notes


[![pipeline status](https://gitlab.com/Mercotui/kernel_workshop/badges/main/pipeline.svg)](https://gitlab.com/Mercotui/kernel_workshop/-/commits/main)
[![coverage report](https://gitlab.com/Mercotui/kernel_workshop/badges/main/coverage.svg)](https://gitlab.com/Mercotui/kernel_workshop/-/commits/main)

These are the notes I wanted to take during the workshop.

## Installing VM

To get gnome boxes to work with proper file sharing and 3D acceleration,
I installed the whole virtualization group:

```bash
sudo yum group install --with-optional virtualization
```

In the guest os make sure the spice tools are installed:

```bash
$ yum list --installed | grep spice
...
spice-vdagent.x86_64
spice-webdavd.x86_64
```

Then the shared folders should be accessible in:

```
dav://localhost:9843
```

## Progress

This is how far I have progressed in the workshop:

* [x] [Workshop Goals](../README.md#workshop-goals)
* [x] [Workshop Description](../README.md#workshop-description)
* [x] [Configure your Virtual Machine (VM)](../README.md#configure-your-virtual-machine-vm)
* [ ] [Update the kernel and download some useful packages](../README.md#update-the-kernel-and-download-some-useful-packages)
* [ ] [Build a vanilla upstream kernel](../README.md#build-a-vanilla-upstream-kernel)
    * [ ] [Cloning (downloading) the upstream kernel tree from GitHub.com](../README.md#cloning-downloading-the-upstream-kernel-tree-from-githubcom)
* [ ] [Configuring the kernel](../README.md#configuring-the-kernel)
* [ ] [Building the kernel](../README.md#building-the-kernel)
* [ ] [Installing the new kernel on your machine](../README.md#installing-the-new-kernel-on-your-machine)
* [ ] [Change the EXTRAVERSION string in the kernel Makefile.](../README.md#change-the-extraversion-string-in-the-kernel-makefile)
* [ ] [Building (out of the tree) kernel modules](../README.md#building-out-of-the-tree-kernel-modules)
* [ ] [These are the bare-bones to start with building out of tree kernel modules:](../README.md#these-are-the-bare-bones-to-start-with-building-out-of-tree-kernel-modules)
    * [ ] [Suggestions to expand the skeleton module, as bonus exercises:](../README.md#suggestions-to-expand-the-skeleton-module-as-bonus-exercises)
* [ ] [Running the new kernel with QEMU.](../README.md#running-the-new-kernel-with-qemu)
    * [ ] [Preparing to run the kernel](../README.md#preparing-to-run-the-kernel)
* [ ] [Running the kernel with QEMU](../README.md#running-the-kernel-with-qemu)
* [ ] [Debugging the kernel with GDB](../README.md#debugging-the-kernel-with-gdb)
    * [ ] [Changing the QEMU command](../README.md#changing-the-qemu-command)
    * [ ] [Starting GDB](../README.md#starting-gdb)
* [ ] [Stop the kernel and find the corresponding source](../README.md#stop-the-kernel-and-find-the-corresponding-source)
* [ ] [Kernel Patching](../README.md#kernel-patching)
    * [ ] [Creating a kernel patch](../README.md#creating-a-kernel-patch)
    * [ ] [Applying and removing a patch to the unchanged kernel.](../README.md#applying-and-removing-a-patch-to-the-unchanged-kernel)
* [ ] [Defining a new System Call and creating a patch to distribute it.](../README.md#defining-a-new-system-call-and-creating-a-patch-to-distribute-it)
    * [ ] [Implementation](../README.md#implementation)
    * [ ] [Rebuilding the kernel to add the new system call.](../README.md#rebuilding-the-kernel-to-add-the-new-system-call)
* [ ] [Adding page-cache hit/miss counters to the kernel](../README.md#adding-page-cache-hitmiss-counters-to-the-kernel)
* [ ] [Implementation](../README.md#implementation-1)
