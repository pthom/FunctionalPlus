set -e

# this scripts install Xfvb (virtual X server) + fluxbox (basic wndow manager) + x11vnc
# /start_x_vnc.sh enables to start everything, and then the screen can be accessed via VNC (on the localhost)
# In order to be able to connect to vnc, the container has to be launched like this:
#   > docker run --name activisu_docker_builder -p 5900:5900 -it -v $(pwd)/../..:/activisu_src activisu_docker_builder_image  /bin/bash
# (where -p 5900:5900 means "forward the 5900 port of the container to the port 5900 of the host machine")


# Install vnc, xvfb in order to create a 'fake' display 
apt-get update && apt-get install -y x11vnc xvfb icewm
mkdir ~/.vnc
# Setup a password
x11vnc -storepasswd ivs ~/.vnc/passwd
echo 'export DISPLAY=:0' >> ~/.bashrc

# /start_x_vnc.sh will start a "X server + vnc server + window manager" accessible via VNC
# -> you can connect to it via VNC on the port 5900 of the localhost
echo "Xvfb -screen 0 1280x1024x16 & sleep 1 && icewm & x11vnc -forever" > /start_x_vnc.sh 
chmod +x /start_x_vnc.sh


