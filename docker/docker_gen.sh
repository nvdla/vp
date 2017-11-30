#! /bin/bash

docker_dns="/etc/docker/daemon.json"     
if sudo test -f $docker_dns ; then
    echo "File $docker_dns exists... skip to update the script, please make sure the DNS ip works fine"
else
    echo "File $docker_dns does not exist."
    sudo touch $docker_dns
    sudo chmod 755 $docker_dns
    echo "Create $docker_dns"
    dns_ipv4s=`nmcli device list | grep 'IP4.DNS' | perl -pe 's/IP4\.DNS\[\d*\]\:\s*//g'`
    dns="\"dns\": []"
    for dns_ipv4 in $dns_ipv4s;
        do dns=`echo $dns | perl -pe 's/\]/\"'$dns_ipv4'\"\, \]/g'`;
    done
    dns=`echo $dns | perl -pe 's/\, \]/\]\n/g'`
    echo $dns
    echo "{" | sudo tee -a $docker_dns > /dev/null
    echo $dns | sudo tee -a $docker_dns > /dev/null
    echo "}" | sudo tee -a $docker_dns > /dev/null
    sudo service docker restart
fi

host_vp_build="/usr/local/vp-build"
if sudo test -d $host_vp_build ; then
    echo "File $host_vp_build exists... Remove it"
    sudo rm -rf $host_vp_build
    echo "Remove Done"
fi

top_path=$PWD
echo "Copy local source code to $host_vp_build ..."
sudo mkdir -p $host_vp_build
sudo cp -rf . $host_vp_build
echo "Find the relative path"
relative=$(perl -MFile::Spec -e 'print File::Spec->abs2rel("'$host_vp_build'","'$PWD'")')
echo "Relative path is $relative"
echo "Build source code in $host_vp_build ..."
sudo chmod -R 777 $host_vp_build
cd $host_vp_build
./scripts/build.sh
echo "Build source Done"

cd $top_path
sudo mkdir -p ./vp-build
sudo mkdir -p ./vp-build/build
sudo mkdir -p ./vp-build/images
sudo mkdir -p ./vp-build/libs/qbox.build/share

echo "Copy back files to $top_path"
sudo cp -rf  $host_vp_build/build/bin $top_path/vp-build/build/
sudo cp -rf  $host_vp_build/build/lib $top_path/vp-build/build/
sudo cp -rf  $host_vp_build/images/linux-4.13.3 $top_path/vp-build/images/
sudo cp -rf  $host_vp_build/conf $top_path/vp-build/
sudo cp -rf  $host_vp_build/scripts $top_path/vp-build/
sudo cp -rf  $host_vp_build/libs/qbox.build/share/qemu $top_path/vp-build/libs/qbox.build/share/

sudo docker build -t vp/ubuntu14.04:1.0 -f ./docker/Dockerfile .
sudo docker save vp/ubuntu14.04:1.0 > ./docker/ubuntu_vp.tar
echo "Docker Image is created as ./docker/ubuntu_vp.tar"
echo "Remove Directory $host_vp_build ..."
sudo rm -rf $host_vp_build
echo "Remove Directory $host_vp_build Done"
echo "Remove Directory $top_path/vp-build ..."
sudo rm -rf $top_path/vp-build
echo "Remove Directory $top_path/vp-build Done"