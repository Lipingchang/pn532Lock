var mqtt = require('mqtt')
var client = mqtt.connect('mqtt://10.66.4.186');

const  logWatcherID = "2";
const  myuploadTopic = 'logWatcher'; 
const  myHearBeatTopic = "heartBeat"; 
const  heartBeatInterval = 5 * 1000; 				// 心跳包　间隔

client.on('connect',function(){
	client.publish("online suqare",logWatcherID+" is ONLINE!") // 上线广播
	client.subscribe('pn532_boardcast');					// 命令监听频道

	setInterval(function (){								// 心跳包频道
		client.publish(
			myHearBeatTopic,
			JSON.stringify({
					"watcherID":logWatcherID
				}
			)
		)},
		heartBeatInterval
	);
});

client.on('message',function(topic,message) {
	console.log('mqtt:',topic,message.toString());
})


const fs = require('fs');
var fileName = '../logfile.txt'

let passType = ["","通过","拒绝"];
let userType = ["","主人","客人","未知"]; 

// 监视文件：
fs.watch(
	fileName,
	(function () {
		let lastfileTimeStamp= new Date();
		return function (argument) {
			console.log("file:",fileName,"change");
			data = fs.readFileSync(fileName,"ascii");
			data = data.split("\n");

			let dataLen = data.length;
			if( dataLen > 2 ){
				let k = data[dataLen-2].split(" ");
				let sendData = {};
				sendData["watcherID"] = logWatcherID;

				sendData.username = data[dataLen-3];
				sendData.user_t = userType[parseInt(k[0])];
				sendData.pass_t = passType[parseInt(k[1])]; 
				sendData.userID = parseInt(k[2]); 
				sendData.logtime = new Date(parseInt(k[3])*1000);

				if( sendData.logtime.getTime() != lastfileTimeStamp.getTime() ){
					console.log( sendData );
					lastfileTimeStamp = sendData.logtime;
					client.publish( 
						myuploadTopic,
						JSON.stringify(sendData) 
					);
				}
			}else{
				console.log("文件不够长");
			}
		};
	})()
);

	// char userName[255];
	// userType user_t;
	// passType pass_t;
	// int userID;
	// time_t rawtime;

	// enum userType{
	// 	Master_e = 1,
	// 	Guest_e = 2,
	// 	Unknow_e = 3
	// };
	// enum passType{
	// 	Pass_e = 1,
	// 	Reject_e =2
	// };