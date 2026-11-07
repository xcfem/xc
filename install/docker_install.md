# Docker install process

There is a prebuilt Docker image for convenience. You can find it on [Docker Hub](https://hub.docker.com/r/engthiago/xcfem).

**Warning**: To run the image, it is recommended to install Docker in "[rootless mode](https://docs.docker.com/engine/security/rootless/)" in order "to mitigate potential vulnerabilities in the daemon and the container runtime". See below for the technical explanation.

Windows users who want to run containers in rootless mode can use Podman as an alternative to Docker Desktop. See the [Podman installation instructions](./podman_install.md) for setup and how to select rootless mode.

After reading the warning, proceed:

- **Windows:** Install [Docker Desktop](https://docs.docker.com/desktop/setup/install/windows-install/).
- **Linux:** Install Docker Desktop or Docker Engine and follow the [rootless mode guide](https://docs.docker.com/engine/security/rootless/).

Security implications: 

> On Linux, Docker Engine normally runs with root privileges, which give
> it broad control over the system. For more information, see the
> [OWASP Docker Security Cheat Sheet](https://cheatsheetseries.owasp.org/cheatsheets/Docker_Security_Cheat_Sheet.html).

Additional notes on networking:
> Docker Engine also changes host network and firewall settings for
> container networking. During testing on Fedora, switching to
> rootless Docker resolved a virtual machine connectivity issue.

## Option 1: Run the existing container

The below installation process works on **Linux and Windows**, ~MacOsX~ has not been tested.

To get started, enter your `Powershell` on Windows or `Bash` on Linux, then navigate to a directory you which you want to keep your files and create a directory called `xc`, you can use the below code to do that:
```bash
cd ~ && mkdir xc && cd xc
```
The `cd ~` command will navigate to your home directory, it works on both `Powershell` and `Bash`. You can choose to use a different directory, the process should work just the same.

Now run the image, docker will automatically pull (download) and run the image for you.
```bash
docker run -it --mount "type=bind,source=${PWD},target=/home/ubuntu" --name xcfem engthiago/xcfem
```
The `--mount` argument will bind the current working directory to the `/home/ubuntu` directory inside of the docker container, making it easy for you to run these files on XC and work with source control without the need to include XC source code.

After Docker finish with its thing, you should be inside of the docker container with XC environment ready to be used. If everything worked correctly you will see the following on your terminal:
> root@d555144909db:/home/ubuntu#

...where `d555144909db` is the id Docker has assigned your container.

Any files and folders created on the `home/ubuntu` directory will be reflected on the `xc` folder in the host environment, and vice versa.

> [!NOTE]This Image will always dettach a specific commit from the repository to guarantee the environment works, if you wish to update to the latest version of XC, you can switch to the master branch and follow the installation instructions found on xc official documentation.

*Optional:* If you wish to run installation verification tests your can run the following command inside of the container:
```bash
cd /xc_install/xc/verif && sh run_verif.sh && cd /home/ubuntu
```

To exit the container, simply run `exit` in your terminal, just like any other docker container:
```bash
exit
```

After the first run, the `docker run` command will not work, because the container name is in use, simply restart and re-attach to the terminal using:
```bash
docker start -ai xcfem
```
### Optional: Updating XCFEM source code and installation

If you wish to update XC version go to the install folder and checkout the master branch
```bash
cd /xc_install && git checkout master
```
Then, follow the instructions on the official installation process for XC [link](https://github.com/xcfem/xc/blob/master/install/install.linux.md)

## Option 2: Advanced

The source code for the Docker image can be found [here](./Docker/Dockerfile).

You can build it navigating to its local folder and running Docker build command:
```bash
docker build -t my-xc-image .
```

There are some optinal arguments that can be used when build:
```bash
docker build -t my-xc-image --build-arg MAKE_WORKERS=4 --build-arg RUN_TESTS=1 --build-arg XC_COMMIT_ID=0081f13 .
```

...where

`MAKE_WORKERS` = `4` means [Make](https://www.gnu.org/software/make/) will run with 4 process (threads).

`RUN_TESTS` = `1` means testing verification code for XC will be run after the installation process is done, `0` is the default, which ignores this step.

`XC_COMMIT_ID` = `0081f13` is the commit id from XC repository that will be used for building the image and its source code. This guarantees a specific version of XC to the image.
