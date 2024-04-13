WorkerScript.onMessage = function(msg) {
    if(msg.item.favicon === "") {
        msg.item.favicon = "qrc:/icons/default"
    }
    else {
        const xhr = new XMLHttpRequest();
        xhr.open('GET', msg.item.favicon);
        xhr.setRequestHeader('Content-Type', 'image/png');
        xhr.send();
        xhr.onload = function() {
          if(xhr.status !== 200) {
              msg.item.favicon = "qrc:/icons/default"
          }
        }
        xhr.onerror = function() {
            msg.item.favicon = "qrc:/icons/default"
        }
    }
    msg.model.append(msg.item)
    msg.model.sync()
    WorkerScript.sendMessage(msg)
}
