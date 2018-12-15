const fs = require('fs');
var fileName = '../logfile.txt'
fs.watch(fileName,
	(function () {
		return function (argument) {
			console.log("file:",fileName,"change");
			data = fs.readFileSync(fileName,"ascii");
			console.log(data);
		};
	})()
);
