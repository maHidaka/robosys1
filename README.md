# robosys1

## 概要
raspberry pi 4において簡単なGPIO操作を行うためのデバイスドライバ

## 動作説明
GPIO4ピンの状態を読み、その状態によってGPIO25ピンへ電圧出力を変化させる
[![](http://img.youtube.com/vi/JqhvszT88Q4/0.jpg)](http://www.youtube.com/watch?v=JqhvszT88Q4 "")

## 動作環境
- OS : ubuntu20.04
- H/W :Raspberry pi 4 ModelB 4GB
## テスト用回路

![55e4ae3b74cfb4e730716b3b7b24c0bb](https://user-images.githubusercontent.com/50083939/102338712-8eb9ec80-3fd7-11eb-80fe-07552973571e.png)

## テスト方法

ビルド
```
$ make
```
ドライバを実装
```
$ sudo insmod myled.ko
```

権限を変更
```
$ sudo chmod 666 /dev/myled0
```

適当な文字を入力<br>
入力するとGPIO4ピンの状態に応じてGPIO25ピンの電圧が変化する<br>
GPIO4がhighのときGPIO25はlowとなる<br>
多少タイムラグが有るためGPIO4に印加する電圧はしばらく保持する必要がある
```
$ echo hoge > /dev/myled0
```

ドライバを消去
```
$ sudo rmmod myled
```
