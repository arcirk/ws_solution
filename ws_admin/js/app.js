let Client_ = null;

function utf8_to_b64(str) {
    let result;
    try {
        result = window.btoa(unescape(encodeURIComponent(str)));
    }catch (e) {
        result = str;
    }
    return result;
}

function b64_to_utf8(str) {
    let result;
    try {
        result = decodeURIComponent(escape(window.atob(str)));
    }catch (e) {
        result = str;
    }
    return result;
}

class Message{

    constructor(uuid, name, uuid_channel, message){
        this.uuid = uuid;
        this.name = name;
        this.message = message;
        this.uuid_channel = uuid_channel;
    }
}

function getMessage(response){

    try {
        return JSON.parse(response);
    }catch (e) {
       return new Message(null, "admin", null, response);
    }

}
function sendEx(message, client){
    let userName = document.getElementById("userName").value;
    let data = utf8_to_b64(JSON.stringify(new Message(userName, userName, "", message)))
    Client_.client.send("cmd " + data);
    //this.client.send(document.getElementById("userName").value + ": " + document.getElementById("sendMessage").value);
    document.getElementById("sendMessage").value = "";
}

class WebSocketClient{

    constructor ( uri )
    {
        console.log("Initializing WebSocketClient");
        this.uri = uri;
        this.client = null;
        this.active = false;
    }
    connect(){
        let show = function (message_){
            let message__ = b64_to_utf8(message_);

            const msg_ = document.getElementById("messages");
            let obj = getMessage(message__);
            msg_.innerText += obj.name + ": " + obj.message + "\n";
            msg_.scrollTop = msg_.scrollHeight - msg_.clientHeight;
        }
        this.client = new WebSocket(this.uri);
        this.client.onopen = function(ev) {
            this.active = true;
            show("[connection opened]");
            sendEx("set_name " + document.getElementById("userName").value)
        };
        this.client.onclose = function(ev) {
            this.active = false;
            show("[connection closed]");
        };
        this.client.onmessage = function(ev) {
            if (ev.data !== '\n')
                show(ev.data);
        };
        this.client.onerror = function(ev) {
            this.active = false
            show("[error]");
            console.log(ev);
        };
    }

    disconnect() {
        this.client.close();
    }

    send() {
        let userName = document.getElementById("userName").value;
        let message  = document.getElementById("sendMessage").value;
        let data = utf8_to_b64(JSON.stringify(new Message(userName, userName, "", message)))
        this.client.send("cmd " + data);
        //this.client.send(document.getElementById("userName").value + ": " + document.getElementById("sendMessage").value);
        document.getElementById("sendMessage").value = "";
    }
}

function sendMessage_onkeyup(ev) {
    ev.preventDefault();
    if (ev.keyCode === 13) {
        send_onclick();
    }
}

function isDisclosed(){
    let sidenav = document.getElementById("mySidenav");
    return  (sidenav.style.width === "320px");
}

function send_onclick() {
    Client_.send();//document.getElementById("userName").value + ": " + document.getElementById("sendMessage").value);
    document.getElementById("sendMessage").value = "";
}


function connect_onclick(){

    if (Client_ != null) {
        Client_.disconnect();
    }
    Client_ = new WebSocketClient(document.getElementById("uri").value);
    Client_.connect();
}

function disconnect_onclick(){
    Client_.disconnect();
}

function openNav() {
    if (!isDisclosed()){
        document.getElementById("mySidenav").style.width = "320px";
        document.getElementById("main").style.marginLeft = "320px";
    }else
    {
        document.getElementById("mySidenav").style.width = "80px";
        document.getElementById("main").style.marginLeft= "80px";
    }

}

function closeNav() {
    document.getElementById("mySidenav").style.width = "80px";
    document.getElementById("main").style.marginLeft= "80px";
}

closeNav() ;
