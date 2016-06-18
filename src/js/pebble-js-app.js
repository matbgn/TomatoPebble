var work_time = {};

Pebble.addEventListener('ready',
  function(e) {
    console.log('JavaScript app ready and running!');
  }
);

Pebble.addEventListener('appmessage', function(e) {
  console.log('AppMessage received!');
});

Pebble.addEventListener("showConfiguration", function() {
  console.log("showing configuration");
  Pebble.openURL('http://at3flo.ch/tomato-conf.html?'+encodeURIComponent(JSON.stringify(work_time)));
});

Pebble.addEventListener('webviewclosed',
  function(e) {
    work_time = JSON.parse(decodeURIComponent(e.response));
    work_time.KEY_WORK_TIME = parseInt(work_time.KEY_WORK_TIME);
    console.log('Configuration window returned: ' + JSON.stringify(work_time));
    Pebble.sendAppMessage(work_time);
  }
);