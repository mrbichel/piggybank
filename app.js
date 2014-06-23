/**
 * Module dependencies.
 */

var express = require('express')
  , http = require('http')
  , path = require('path');

var redis = require('redis');

var AMOUNT = 0;
var GOAL = 1000;

/*var MongoClient = require('mongodb').MongoClient;

// Connect to the db
MongoClient.connect("mongodb://nodejitsu:2074ef76478e127b96b4461c064ae623@linus.mongohq.com:10052/nodejitsudb8214054855", function(err, db) {
  if(err) { return console.dir(err); }
  
    var collection = db.collection('gris');
    var doc1 = {'goal': 2000, "amount": 0};
    collection.insert(doc1);
    
  }
});*/

var app = express();

app.configure(function(){
  app.set('port', process.env.PORT || 4000);
  app.set('views', __dirname + '/views');
  app.set('view engine', 'jade');
  app.use(express.favicon());
  app.use(express.logger('dev'));
  app.use(express.bodyParser());
  app.use(express.methodOverride());
  app.use(app.router);
  app.use(express.static(path.join(__dirname, 'public')));
  
});

app.configure('development', function(){
  app.use(express.errorHandler());
  
});

app.get('/', function(req, res) {
  
  /*client.hgetall("gris.1", function(err, reply) {
        
    if (err) {
      console.log(err);
    } else if(reply) {
      res.render('index', { gris: reply });
    } else {
    }
  });*/
  
  res.render('index', { gris: {amount: AMOUNT, goal: GOAL} });
  
});

/*app.get('/new', function(req, res) {
  client.hmset("gris.1", {"amount": "0", "goal": "100", "name": "Power Rangers"});
  res.redirect("/");
});*/

app.get('/reset', function(req, res) {
  /*client.hgetall("gris.1", function(err, reply) {
    if (err) {
      console.log(err);
    } else if(reply) {
      reply.amount = "0";
      client.hmset("gris.1", reply, function () { 
        res.redirect("/");
      });    
    } 
  });*/
  
  AMOUNT = 0;
  res.redirect("/");
  
});

app.get('/data', function(req, res) {
  
  /*client.hgetall("gris.1", function(err, reply) {
    if (err) {
      console.log(err);
    } else if(reply) {
      res.send(reply);
    } 
  });*/
  
  res.set({
    'Content-Type': 'text/plain',
  })
  
  res.send("P"+Number(AMOUNT/GOAL*100));
  
});



app.post('/addmoney', function(req, res) {  
  /*client.hgetall("gris.1", function(err, reply) {
    if (err) {
      console.log(err);
    } else if(reply) {
      var newAmount = Number(reply.amount) + Number(req.body.coins);
      console.log(newAmount);
      reply.amount = String(newAmount);
      client.hmset("gris.1", reply, function () { 
        res.redirect("/");
      });    
    } 
  });*/
  
  AMOUNT += Number(req.body.coins);
  res.redirect("/");
   
});

app.post('/edit', function(req, res){
  
  // insert new pig
  /*client.hgetall("gris.1", function(err, reply) {
    if (err) {
      console.log(err);
    } else if(reply) {
      
      reply.goal = req.body.goal;    
      client.hmset("gris.1", reply, function () { 
        res.redirect("/");
      });    
    } 
  });*/
  
  GOAL = req.body.goal;
  res.redirect("/");
  
});


http.createServer(app).listen(app.get('port'), function(){
  console.log("Express server listening on port " + app.get('port'));
});
