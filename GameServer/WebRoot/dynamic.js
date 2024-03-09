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

    var ServerTimeClock = function(initialServerTime, serverTimeOffset) {
        if(serverTimeOffset === null) {
            this._serverTimeOffset = 0;
        } else {
            this._serverTimeOffset = serverTimeOffset * 1000;
        }

        this._clientToServerTimeOffset = Date.now() - (initialServerTime * 1000);

        console.log("Server-client time discrepancy:", this._clientToServerTimeOffset, "milliseconds");

        this._updateTimeDisplay();

        var classes = document.getElementById('server-time').classList;
        classes.add('operational');

        if(this._serverTimeOffset != 0) {
            classes.add('time-traveling');
        }

        setInterval(this._updateTimeDisplay.bind(this), 1000);
    };

    ServerTimeClock.prototype._updateTimeDisplay = function() {
        var currentRealWorldTime = new Date(Date.now() - this._clientToServerTimeOffset);
        var currentServerTime = new Date(Date.now() - this._clientToServerTimeOffset + this._serverTimeOffset);

        document.getElementById('real-world-time-value').innerText = currentRealWorldTime.toLocaleString();
        document.getElementById('server-time-value').innerText = currentServerTime.toLocaleString();
    };

    new ServerCliComponent(document.getElementById('server-cli'));

    if(typeof document.body.dataset.serverTime !== 'undefined') {
        var serverTime = parseInt(document.body.dataset.serverTime, 10);
        var timeOffset = null;

        if(typeof document.body.dataset.serverTimeOffset !== 'undefined') {
            timeOffset = parseInt(document.body.dataset.serverTimeOffset, 10);
        }

        new ServerTimeClock(serverTime, timeOffset);
    }
})();
