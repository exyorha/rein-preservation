<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns="http://www.w3.org/1999/xhtml">

    <xsl:output method="xml" version="1.0" encoding="utf-8"
            doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"
            doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd" />

    <xsl:template match="/">
        <html lang="en">
            <head>
                <link rel="stylesheet" type="text/css" href="/style.css" />
                <meta name="viewport" content="width=device-width, initial-scale=1.0" />
            </head>

            <body>
                <xsl:apply-templates />

                <form id="server-cli">
                    <div class="log" />

                    <input class="commandline" type="text" name="command" />

                    <div class="overlay-message connecting-overlay-message">
                        <span>Connecting to the server...</span>
                    </div>

                    <div class="overlay-message disconnected-overlay-message">
                        <span>Disconnected from the server. Please reload the page.</span>
                    </div>
                </form>

                <script type="text/javascript" src="/dynamic.js"></script>
            </body>
        </html>
    </xsl:template>
</xsl:stylesheet>

