/**
 * Module dependencies.
 */

var express = require('express')
    , http = require('http')
    , path = require('path')
    , iniparser = require('iniparser')
    , serialport = require("serialport").SerialPort;


var config = iniparser.parseSync('./config.ini');

var env = process.env.NODE_ENV || 'dev'; //startup nodejs with e.g:  NODE_ENV= node server.js
/**
 * Serial Port Setup.
 */

var portName = null;


if(process.platform === 'win32'){portName = config[env]['windows'];
} else if ( process.platofrm === 'darwin'){portName = config[env]['macOSX'];
} else {portName = config[env]['linux'];
}

var readData = ''; //Array to hold the values read in from the port

var sp = new serialport(portName, {
    baudRate: 4800,
    dataBits: 8,
    parity: 'none',
    stopBits: 1,
    flowControl: false,
});

/**
 * Express Setup.
 */

var app = express();

app.configure(function(){
    app.set('port', process.env.PORT || 8000);
    app.set('views', __dirname + '/views');
    app.set('view engine', 'jade');
    app.use(express.favicon());
    app.use(express.logger('dev'));
    app.use(express.bodyParser());
    app.use(express.methodOverride());
    app.use(app.router);
    app.use(express.static(__dirname));
});

app.configure('development', function(){
    app.use(express.errorHandler());
});

/**
 * Actual Server Code
 */

var server = http.createServer(app).listen(app.get('port'), function(){
    console.log("Express server listening on port " + app.get('port'));
});

var io = require('socket.io').listen(server);
console.log("Serial port server listening on " + portName )

/**
 * Server and socket started, below are all my listeners and emitters
 */

io.sockets.on('connection', function(socket){
    console.log("Socket connected");
    socket.emit('connected', 123);

    socket.on('sendSerial', function(data) {
        data = data.replace(/["']/g, "");
        console.log("Client sent us: " + data + " to send to serial");

        sp.write(data, function (err, bytesWritten) {
            console.log('bytes written:', bytesWritten);
        });
    });
    socket.on('newLine', function(){
        sp.write('\u000D' + '\u000A');
    })

    sp.on('data', function (data) {
        console.log(data);
        var mbRec = new Buffer(data, 'utf')
        console.log(mbRec);
        mbRec = mbRec.toString('hex');
        console.log(mbRec);
        socket.emit('serialEvent', mbRec);
    });

    sp.on('close', function (err) {
        console.log('port closed');
    });

    sp.on('error', function (err) {
        console.error("error", err);
    });

    sp.on('open', function () {
        console.log('port opened...');
    });
})

var fs = require('fs')

exports.index = function(req, res){
    res.writeHead(200, {'Content-Type': 'text/html'});
    var contents = fs.readFileSync("/index.html", "UTF-8");
    res.end(contents);
};



