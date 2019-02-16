meta-allwinner-hx
----

This meta layer is mainly a mix of `meta-sunxi` and `armbian`.

This meta layer supports only boards with the allwinner H2, H3 and H5 cpus.
The boards that are supported are the same ones that supported in
[armbian](https://www.armbian.com/download/). The current supported list is
the following ones:

- nanopi-duo
- nanopi-k1-plus
- nanopi-m1
- nanopi-m1-plus
- nanopi-neo
- nanopi-neo2
- nanopi-neo-air
- nanopi-neo-core2
- nanopi-neo-plus2
- orange-pi-one
- orange-pi-pc
- orange-pi-pc-plus
- orange-pi-prime
- orange-pi-win
- orange-pi-zero
- orange-pi-zero-plus
- orange-pi-zero-plus2

The way this meta layer works is that it uses the u-boot and kernel patches
from the armbian distro. The patches are located in:
* `u-boot`: recipes-bsp/u-boot/files/patches
* `kernel`: recipes-kernel/linux/linux-stable/patches

Also the patcher is ported from armbian and actually is the same for u-boot
and the kernel, but it's in two places now:
* `u-boot`: recipes-bsp/u-boot/files/do_patch.sh
* `kernel`: recipes-kernel/linux/linux-stable/do_patch.sh

Maybe at some point this will be converted in a `bbclass`.

> Note: Not all of the above boards are tested, because I don't have them.
I've only tested `nanopi-k1-plus` and `nanopi-neo2`.

## Updates:
* `17.2.2019`
    * Added kernel 4.19.21 for SMP
    * Added kernel 4.19.15 for PREEMPT-RT
    * Updated u-boot to 2018.11
    * Knowing issues:
        * For PREEMPT-RT kernels the `CONFIG_DEBUG_SUNXI_UART0` is missing, so no debug output during boot

## How to use the layer
Create a folder for your project, then create a folder inside and name it
`sources`. You _have_ to use that name.

Then `git clone` this repo inside with `poky` and `meta-openembedded`.

```sh
cd sources
git clone git@bitbucket.org:dimtass/meta-allwinner-hx.git
git clone --depth 1 -b sumo git://git.yoctoproject.org/poky
git clone --depth 1 -b sumo git@github.com:openembedded/meta-openembedded.git
```

Then from the `top` directory that includes the sources run this command:
```sh
cp sources/meta-allwinner-hx/scripts/setup-environment.sh .
cp sources/meta-allwinner-hx/scripts/flash_sd.sh .
```

Then your top dir contects should look like this:
```sh
flash_sd.sh
setup-environment.sh
sources
```

Then depending on the board you have, you need to set the `MACHINE` variable to the
correct name. To list the names of supported boards you can the script like this:
```sh
source ./setup-environment.sh build
```

Now in your `build/conf/local.conf` file you can choose which kernel you want to build.
By default the `linux-stable` 4.19 version is build. In case you want to build the RT
kernel then see next section.

This will result in error, but it will also print the supported boards. Therefore,
for `nanopi-k1-plus`, you can run:
```sh
MACHINE=nanopi-k1-plus source ./setup-environment.sh build
```

Then to start building the image run as prompted:
```sh
bitbake allwinner-image
```

In this case this will create a `.wic.bz2` image inside your `build/tmp/deploy/images/nanopi-k1-plus`.

## Kernels
By default the linux-stable 4.19 kernel is used but this layer also supports the
4.14 kernel and also the PREEMPT-RT patches for both kernels. As you can imagine,
I haven't tested all the kernels with all the boards, so there might be some cases
that something doesn't work properly.

To enable another kernel you need to edit your `build/conf/local.conf` and select
the kernel you want. The available options are:

* linux-stable 4.19
```
PREFERRED_PROVIDER_virtual/kernel = "linux-stable"
PREFERRED_VERSION_linux-stable = "4.19%"
```
* linux-stable-rt 4.19
```
PREFERRED_PROVIDER_virtual/kernel = "linux-stable-rt"
PREFERRED_VERSION_linux-stable-rt = "4.19%"
```
* linux-stable 4.14
```
PREFERRED_PROVIDER_virtual/kernel = "linux-stable"
PREFERRED_VERSION_linux-stable = "4.14%"
```
* linux-stable-rt 4.14
```
PREFERRED_PROVIDER_virtual/kernel = "linux-stable-rt"
PREFERRED_VERSION_linux-stable-rt = "4.14%"
```

## Overlays
This layer supports overlays for the allwinners boards. In order to use them you need
to edit the `recipes-bsp/u-boot/files/allwinnerEnv.txt` file or even better create
a new layer with your custom cofiguration and override the `allwinnerEnv.txt` file by
pointing to your custom file in your `recipes-bsp/u-boot/u-boot_2018.11.bbappend`
with this line:

```sh
SRC_URI += "file://allwinnerEnv.txt"
```

Of course, you need to create this file and place it in your layer file folder.
In that file you need to edit it and add the overlays you need, for example:

```sh
extra_bootargs=
rootfstype=ext4
verbosity=d
overlays=sun8i-h3-i2c0 sun8i-h3-spi-spidev
```

## WiFi networking
If your board has only a wifi network then you can add the `SSID` and the `PSK` password
in the `build/conf/local.conf` and build the image. You can remove the comment on those
two lines in the `build/conf/local.conf` and the proper values for your network.
```sh
SSID = "YOUR_SSID"
PSK = YOUR_SSID_PASSWORD"
```

For some reason it seems that the `wpa_supplicant@wlan0.service` service is not installed
and started automatically. Because of that, on the first boot after the SD flash you need
to run the following command manually and after that it works fine forever.
```sh
systemctl enable wpa_supplicant@wlan0
```

## Flashing the image
After the image is build, you can use `bmaptool` to flash the image on your SD card.
To this you first need to install `bmap-tools`.
```sh
sudo apt-get install bmap-tools
```

Then you need to run `lsblk` to find the device path of the SD and only after
you verified the correct device then from your top directory run this:
```sh
sudo ./flash_sd.sh /dev/sdX
```

If you want to do the steps manually then:
```sh
sudo umount /dev/sdX*
sudo bmaptool copy <.wic.bz2_image_path> /dev/sdX
```

Of course you need to change `/dev/sdX` with you actuall SD card dev path.

## Why bmap-tools and wic images?
Well, wic images are a no-brainer. You can create a 50GB image, but this image
probably won't be that large really. Most of the times, the real data bytes in
the image will be from a few hundreds MB, to maybe 1-2 GB. The rest will be
empty space. Therefore, if you build a binary image then this image will be
filled with zeros. You will also have to use `dd` to flash the image to the SD
card. That means that for a few MBs of real data, you'll wait maybe more than
an hour to be written in the SD. Wic creates a map of the data and creates an
image with the real binary data and a bmap file that has the map data. Then,
bmaptool will use this bmap file and create the partitions and only write the
real binary data. This will take a few seconds or minutes, even for images that
are a lot of GBs.

For example the default image size for this repo is 13.8GB but the real data
are ~62MB. Therefore, with a random SD card I have here the flashing takes
~14 secs and you get a 14GB image.

## Notes
You can also build the `core-image-minimal` using this meta layer. But for some
reason when you'll get the login prompt, then the `root` account doesn't work.
This problem seems to be quite common, though.

The `allwinner-image` will install a service that forces the `perfomance` governor
for all the cores by default. If you want to disable this, then you can remove
the `allwinner-performance` entry from the `meta-allwinner-hx/recipes-images/images/allwinner-image.bb`
image file. Or you can disable the service after you boot with
```sh
systemctl disable allwinner-performance
```

> Once again, I haven't test this with all the boards, as I don't have them, but
I expect that it should work also for the others!

