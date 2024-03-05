(function() {
    var ServerCliComponent = function(element) {
        this._element = element;

        var url = new URL('/server-cli', document.location.href)

        if(url.protocol == "https:") {
            url.protocol = "wss:";
        } else {
            url.protocol = "ws:";
        }

        this._socket = new WebSocket(url);
    };

    new ServerCliComponent(document.getElementById('server-cli'));
})();
