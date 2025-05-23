# lib_weather (wttr.in 웹서버를 통한 날씨 라이브러리)

### ODROID-C5 (2025-05-09)
* Linux OS Image : [factory-odroidc5-0307.img (odroidh server)](http://192.168.0.224:8080/S905X5M/ODROID-C5/Ubuntu/ubuntu-22.04-server-odroidc5-20250509.img.xz)

### Install package
```
// ubuntu package install
root@server:~# apt install build-essential vim ssh git python3 python3-pip ethtool net-tools usbutils i2c-tools overlayroot nmap evtest htop cups cups-bsd iperf3 alsa libcurl4-openssl-dev libcjson-dev

// system reboot
root@server:~# reboot

root@server:~# uname -a
Linux server 5.15.153-odroid-arm64 #202505121444~factory SMP PREEMPT Mon May 12 14:44:01 KST 2025 aarch64 aarch64 aarch64 GNU/Linux
```

### [ChatGPT 사용내역](https://chatgpt.com/) : 기타 발생되는 오류는 알아서 수정함.
* 현재 나의 지역날씨 가져오는 C code
* 날씨 아이콘 말고 텍스트로 표시
* 출력 데이터 숫자 문자 모두 한글로 변경

### 태스트 방법
* ./lib_weather (현 위치 기반의 날씨정보 가져옴)
* ./lib_weather [위도] [경도] (위/경도 위치근처의 날씨 정보 가져옴)
* ./lib_weather [지역명/국가] (지역 또는 국가근처의 날씨 정보 가져옴. 한글 및 영어 사용가능함)
   
### Github setting
```
root@server:~# git config --global user.email "charles.park@hardkernel.com"
root@server:~# git config --global user.name "charles-park"
```

### Clone the reopsitory with submodule
```
root@server:~# git clone --recursive https://github.com/charles-park/lib_weather

or

root@server:~# git clone https://github.com/charles-park/lib_weather
root@server:~# cd lib_weather
root@server:~/lib_weather# git submodule update --init --recursive
```

### Auto login
```
root@server:~# systemctl edit getty@tty1.service
```
```
[Service]
ExecStart=
ExecStart=-/sbin/agetty --noissue --autologin root %I $TERM
Type=idle
```
* edit tool save
  save exit [Ctrl+ k, Ctrl + q]

