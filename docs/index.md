Opencast OBS Studio Plugin
==========================

A Plugin for [OBS Studio](https://obsproject.com/), which allows recorded files
to be uploaded directly to [Opencast](http://opencast.org).

OBS Studio is a free and open source software for video recording and live
streaming. In combination with Opencast and this plugin, lecturers are easily
able to create screen recordings and upload them to Opencast for processing and
distribution alongside their regular lecture recordings.

OBS Studio and this plugin are available both for Linux and Windows.


Installation
------------

On Linux:

1. [Download](https://github.com/elan-ev/opencast-obs-plugin/releases) the latest version of the plugin.

2. Make sure to install tinyxml2 and libconfig. E.g.:

        dnf install tinyxml2 libconfig

3. Extract the zip file into your obs plugin folder. Usually, the location is
   something like `/usr/lib*/obs-plugins/`.


On Windows:

1. [Download](https://github.com/elan-ev/opencast-obs-plugin/releases) the latest version of the plugin.

2. Extract the zip file into your OBS plugin folder:

    - Windows 32-bit: `C:\Program Files (x86)\obs-studio\obs-plugins\32bit\`
    - Windows 64-bit: `C:\Program Files (x86)\obs-studio\obs-plugins\64bit\`

If you want to build the plugin from source, take a look at the [documentation
about building the plugin](building.md)


Configuration
-------------

You need to configure the plugin before being able to use it properly. The
settings dialog for this can be found in the `Tools` menu.

![Tools](img/tools.jpg)

Here you can enter the address of the server and the authentication details.

![settings dialog](img/settingsdialog.jpg)


Usage
-----

When you stop a recording, a dialog for entering metadata for the recording will
appear.

![upload dialog](img/uploaddialog.jpg)

These data will automatically be transferred to your pre-configured Opencast
installation once you hit the upload button.

Do not close OBS Studio while the event is being uploaded, or the process will
be aborted. Once the upload process has finished you will be notified.

![finish](img/finish.jpg)
