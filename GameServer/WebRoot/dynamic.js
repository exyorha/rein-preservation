(function() {
    var ServerCliComponent = function(element) {
        this._element = element;
        this._log = this._element.getElementsByClassName('log')[0]

        var url = new URL('/server-cli', document.location.href)

        if(url.protocol == "https:") {
            url.protocol = "wss:";
        } else {
            url.protocol = "ws:";
        }

        this._socket = new WebSocket(url);
        this._socket.onclose = this._wsclose.bind(this);
        this._socket.onmessage = this._wsmessage.bind(this);
        this._socket.onopen = this._wsopen.bind(this);

        this._element.onsubmit = this._formsubmit.bind(this);
    };

    ServerCliComponent.prototype = {
        _wsclose: function(ev) {
            this._element.classList.remove('connected');
            this._element.classList.add('disconnected');
        },

        _wsmessage: function(ev) {
            ev.data.text().then(this._wsmessagedecoded.bind(this))

        },

        _wsmessagedecoded: function(text) {
            var fullText = this._log.innerText
            fullText += text;

            if(fullText.length > 512 * 1024) {
                fullText = fullText.substr(fullText.length - 512 * 1024, 512 * 1024);
            }

            this._log.innerText = fullText;
            this._log.scrollTop = this._log.scrollHeight;
        },

        _wsopen: function(ev) {
            this._element.classList.add('connected');
        },

        _formsubmit: function(ev) {
            ev.preventDefault();

            var command = this._element.elements.namedItem('command').value;
            if(command.length > 0) {

                this._socket.send(new Blob([ command ]));
            }

            this._element.reset();
        }
    };

    new ServerCliComponent(document.getElementById('server-cli'));
})();
