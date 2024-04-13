WorkerScript.onMessage = function(msg) {
    const xhr = new XMLHttpRequest();
    xhr.open('GET', 'http://de1.api.radio-browser.info/json/tags');
    xhr.setRequestHeader('Content-Type', 'application/json; charset=utf-8');
    xhr.send();
    xhr.onload = function() {
      if(xhr.status != 200) {
        msg.text = xhr.statusText;
        WorkerScript.sendMessage(msg);
      } else {
          var objs = JSON.parse(xhr.responseText.toString());

          objs.forEach(e => {
          if(e.stationcount.valueOf() >= msg.minItems) {
               msg.tags.push(e.name.toString());
          }
        });
        msg.ok = true
        WorkerScript.sendMessage(msg)
      }
    };

    xhr.onerror = function() {
      msg.text = "Request failed";
      WorkerScript.sendMessage(msg)
    };
}
