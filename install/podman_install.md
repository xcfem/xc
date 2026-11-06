# Podman installation with Compose

Run the prebuilt XC image with rootless Podman and a Compose file.

Podman "is designed to be a more secure and lightweight alternative to Docker" 
([source](https://cheatsheetseries.owasp.org/cheatsheets/Docker_Security_Cheat_Sheet.html#podman-as-an-alternative-to-docker)). The XC image is hosted on Docker Hub and is compatible with Podman.

## Install Podman and Compose

### Windows

Install [Podman Desktop](https://podman-desktop.io/downloads), a graphical application for managing Podman. Follow its [Windows installation guide](https://podman-desktop.io/docs/installation/windows-install) to install Podman, set up and start a Podman machine, and install a Compose provider. When creating the machine, leave **Machine with root privileges** disabled to use rootless mode. If you are unsure which provider to choose, use WSL 2.

### Linux

Install [Podman](https://podman.io/docs/installation) for your distribution and a Compose provider such as `podman-compose` or Docker Compose. Optionally, install [Podman Desktop](https://podman-desktop.io/docs/installation/linux-install) for a graphical interface.

## Verify Podman setup

`podman compose` uses an external Compose provider; see the [Podman Compose documentation](https://docs.podman.io/en/latest/markdown/podman-compose.1.html).

Check that Podman is using rootless mode and that Compose is available. Open PowerShell on Windows, or a terminal on Linux. Run these commands one at a time, pressing Enter after each:

```bash
podman info --format '{{.Host.Security.Rootless}}'
podman compose version
```
To copy a command from this guide, select it and press `Ctrl+C`. To paste it into many terminals, press `Ctrl+Shift+V`. Windows Terminal also supports `Ctrl+V`; shortcuts can vary between terminals.

The first command should print `true`, and the second should print a Compose version.  If both checks succeed, continue to [Create the Compose setup](#create-the-compose-setup). 

If the first command prints false, follow the troubleshooting instructions for your operating system below. If either command reports that Podman or Compose is missing, check the installation steps above. If Podman cannot connect on Windows, check that the Podman machine is running in Podman Desktop.
 
#### Select the rootless connection on Windows (Read if rootless is false)

In Podman Desktop, open Settings > Resources and check that the Podman machine is configured without root privileges. Then check that its rootless connection is the default:

List the available connections in PowerShell:

```powershell
podman system connection ls
```

Find the connection for your machine whose URI contains `/run/user/` and whose user is not `root`. For a machine named `podman-machine-default`, this connection is commonly named `podman-machine-default`. The connection ending in `-root` is rootful. Names can differ, so check the list rather than assuming a name.

Set the rootless connection as the default, replacing the example name if yours differs:

```powershell
podman system connection default podman-machine-default
```

Run `podman system connection ls` again to check that its **Default** column is `true`, then repeat the rootless check above. The example using `podman-machine-default-root` in the [Podman Desktop connection guide](https://podman-desktop.io/docs/podman/setting-podman-machine-default-connection) selects a rootful connection, which is not the one wanted here.


#### Use rootless Podman on Linux

Open a terminal as your regular user and run Podman commands without `sudo`. Podman normally runs rootless in that case; there is no separate rootless switch to enable. Containers created with `sudo` belong to the rootful environment and are not shown in your regular user's container list.

Rootless containers also need subordinate user and group ID ranges configured for your account. If rootless Podman reports an ID mapping error, check your distribution's Podman setup instructions and the [Podman rootless documentation](https://docs.podman.io/en/latest/markdown/podman.1.html#rootless-mode). These ranges must be configured without overlapping other users' ranges.

## Create the Compose setup

Create a directory (folder) for your XC files. Open a plain-text editor, paste the following into a new file, and save it inside that folder as `compose.yaml`:

```yaml
services:
  xc:
    image: docker.io/engthiago/xcfem
    stdin_open: true
    tty: true
    volumes:
      - ./workspace:/home/ubuntu
```

Make sure the file is named `compose.yaml`, not `compose.yaml.txt`. In Windows Notepad, select **All files** as the file type when saving.

Create a directory called `workspace` beside `compose.yaml`. Files in it are available in the container at `/home/ubuntu`. The host path is relative to the Compose file.

> **Security:** Mount only the folders needed for your XC work. Processes in the container can read and modify files in these mounts, so avoid mounting your entire home directory or folders containing sensitive files.

To mount multiple project folders instead of `workspace`, replace the existing `volumes` section with the following. This mounts two project folders beside `compose.yaml`:

```yaml
    volumes:
      - ./project-a:/home/ubuntu/project-a
      - ./project-b:/home/ubuntu/project-b
```

Each entry maps a host folder on the left to a container folder on the right. Compose can create a missing host folder, but you can also create it yourself before starting the container.


> On Linux systems enforcing SELinux, the workspace mount may need an appropriate container label. For a workspace used exclusively by this container, the mount can be written as `./workspace:/home/ubuntu:Z`; this relabels the host directory. Do not apply it to a shared or system directory.

## Start XC

In your terminal, use `cd` to enter the folder containing `compose.yaml`. Replace the example path with your folder's actual path; keep the quotes if the path contains spaces.

Windows PowerShell example:

```powershell
cd "C:\Users\your-name\xc"
```

Linux example:

```bash
cd "/home/your-name/xc"
```

Then run these commands one at a time:

```bash
podman compose up -d
podman compose exec xc bash
```

Compose pulls the image if needed. Type `exit` to leave the shell; the container stays running. To open another shell, run the `exec` command again.

To check that XC can be imported, run this command inside the container:

```bash
python -c "import xc; print('XC import OK')"
```

It should print `XC import OK`.

To stop and remove the container:

```bash
podman compose down
```

Files in `workspace` remain on the host. Changes made elsewhere inside the container may be lost when it is removed or recreated.

### Stop Podman on Windows

After `podman compose down`, you can stop the Podman machine from PowerShell to release its resources:

```powershell
podman machine stop
```

Start it again with `podman machine start` before using XC. To stop WSL completely, you can also run `wsl --shutdown`; this stops **all** running WSL distributions, including any unrelated to Podman.

To prevent Podman Desktop from starting automatically when you sign in, open **Settings > Preferences** in Podman Desktop and disable **Start on login**. The Podman engine's automatic startup is a separate preference.



## Optional: Run verification tests

From a shell inside the container:

```bash
cd /xc_install/xc/verif && sh run_verif.sh
```

## Build or update the image (advanced)

See the [Docker guide](./docker_install.md) for the image source and build options. The prebuilt image uses a specific XC commit. To use a more recent version of XC, build an image with a newer `XC_COMMIT_ID` and configure Compose to use the new image. Changes made inside a container do not update the image.
