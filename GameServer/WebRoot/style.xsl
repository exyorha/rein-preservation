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

                <script type="text/javascript" src="/dynamic.js"></script>
            </body>
        </html>
    </xsl:template>

    <xsl:template match="database-page">
        <xsl:attribute name="data-server-time">
            <xsl:value-of select="@current-time" />
        </xsl:attribute>

        <xsl:if test="@time-offset">
            <xsl:attribute name="data-server-time-offset">
                <xsl:value-of select="@time-offset" />
            </xsl:attribute>
        </xsl:if>

        <div class="page-header">

            <div class="title">[re]serve</div>
            <div class="versions">
                Server software: <xsl:value-of select="@gitversion" /><br />
                Master database: <xsl:value-of select="@master-db-version" /><br />
                Asset database: <xsl:value-of select="@octo-version" /><br />
            </div>

            <div id="server-time">
                <div class="time-travel-container">
                    <div class="time-travel-notice">
                        You are time-traveling.
                    </div>
                </div>

                Server time: <span id="server-time-value" /> <br />

                <div class="time-travel-container">
                    Real-world time: <span id="real-world-time-value" />
                </div>
            </div>
        </div>

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
    </xsl:template>

</xsl:stylesheet>

