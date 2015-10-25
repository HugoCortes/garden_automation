# Raspberry Pi 2 Installation

## Initial Raspberry Pi 2 Configuration
1. Expand root partition / set timezone / change password  
`sudo raspi-config` 
2. Update  
`sudo apt-get update`  
`sudo apt-get upgrade`  
3. Change SSH port  
`sudo nano /etc/ssh/sshd_config`  
`sudo ssh restart`  
4. Get address  
`ifconfig`  
5. (Optional) Install Samba to allow RPi on Windows network  
`sudo apt-get install samba samba-common-bin`  
5a. Modify and uncomment workgroup and wins support variable in smb.conf  

    ```
    sudo nano /etc/samba/smb.conf  
        workgroup = your_workgroup_name (typically WORKGROUP)  
        wins support = yes  
    ```
5b. Add the following anywhere in smb.conf (preferably at the end)  

    ```
        [pi]  
            comment= pi  
            path=/  
            browseable=Yes  
            writeable=Yes  
            only guest=no  
            create mask=0777  
            directory mask=0777  
            public=no  
            force user = root  
    ```
5c. (Optional) Change Samba password  
`sudo smbpasswd -a pi`  
5d. Restart Samba (If service not found, restart RPi)  
`sudo service smbd restart`  

## Raspberry Pi 2 Configuration for garden_automation  

1. Installing Apache  
`sudo apt-get install apache2 php5 libapache2-mod-php5`  
1a. (Optional) Change following variables for different port in ports.conf and 000-default  

    ```
    sudo nano /etc/apache2/ports.conf  
        NameVirtual Host  
        Listen  
    ```
    
    ```
    sudo nano /etc/apache2/sites-enabled/000-default  
        VirtualHost  
    ```
1b. Restart apache  
`sudo service apache2 restart`  
2. Install MySQL  
`sudo apt-get install mysql-server mysql-client php5-mysql`  
2a. Install MySQL C Libraries  
`sudo apt-get install libmysqlclient-dev`  
3. Change Root password  
`sudo passwd root`  
`sudo usermod -L root`  
4. Change /var/www ownership
`sudo chown -R pi /var/www`  
5. Install FTP 
`sudo apt-get install vsftpd`  
5a. Modify/Uncomment the following from vsftpd.conf  

    ```
    sudo nano /etc/vsftpd.conf
        anonymous_enable=NO
        local_enable=YES
        write_enable=YES
        
        force_dot_files=YES // Add to bottom
    ```
5c. Restart  
`sudo service vsftpd restart`  
6. Install and build WiringPi (Add -lwiringPi for compile)  

    ```
    sudo git clone git://git.drogon.net/wiringPi  
    cd wiringPi  
    sudo ./build
    ```
6a. Test installation

    ```
    gpio -v  
    gpio readall  
    ```
7. Make and install RF24 (in libraries or pull from github)

    ```  
    cd rpi/libraries/RF24_rpi  
    sudo make  
    sudo make install  
    ```  
7a. Check installation. Look for librf24.so.1 -> librf24.so.1.0  
`sudo ldconfig -v | grep librf`  
8. Install oauth for Ruby Twitter API  
`sudo gem install evernote_oauth`  

Note if pulling from github: 
Raspberry Pi 2 edit. Reverse for Raspberry Pi 1  

    sudo nano {RF24 Directory}/bcm2835.h  
        #define BCM2835_PERI_BASE {ADDRESS} // 0x0x20000000 (RPi 1) or 0x3F000000 (RPi 2)  

Note port forwarding on router:  
http://GobalIP:Port/  
