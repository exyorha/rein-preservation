<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns="http://www.w3.org/1999/xhtml">

    <xsl:output method="xml" version="1.0" encoding="utf-8"
            doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"
            doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd" />

    <xsl:template match="/">
        <html lang="en">
            <head>
                <link rel="stylesheet" type="text/css" href="/databaseviewer-assets/style.css" />
                <meta name="viewport" content="width=device-width, initial-scale=1.0" />
            </head>

            <body>
                <xsl:apply-templates />

                <script type="text/javascript" src="/databaseviewer-assets/dynamic.js"></script>
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
            <a href="/database" class="title">

                <svg xmlns="http://www.w3.org/2000/svg" viewBox="-257 -169 514 441" class="logo">
                    <!-- left 'arm' -->
                    <path d="M -257 68 H -223 V 174 H -107 V 272 H -142 V 209 H -257 Z" fill="white" id="leftarm" />
                    <!-- right 'arm' -->
                    <use href="#leftarm" transform="scale(-1,1)" />

                    <!-- 'stack' -->
                    <path d="M -81 0 H 81 V 35 H -81 Z" id="stackbottom" fill="white" /><!-- bottommost -->
                    <path d="M -55 -85 H 55 V -50 H -55 Z" fill="white" /><!-- middle -->
                    <use href="#stackbottom" transform="translate(0,-169)" /><!-- topmost -->
                </svg>
                [re]serve
            </a>
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

    <xsl:template match="entities">
        <ul class="entity-list">
            <xsl:for-each select="entity">
                <li>
                    <a>
                        <xsl:attribute name="href">/database/<xsl:value-of select="@name" /></xsl:attribute>
                        <xsl:value-of select="@name" />
                    </a>
                </li>
            </xsl:for-each>
        </ul>
    </xsl:template>

    <xsl:template match="entity-list">
        <div class="main-content">
            <h1>
                <a>
                    <xsl:attribute name="href">/database/<xsl:value-of select="@name" /></xsl:attribute>
                    <xsl:value-of select="@name" />
                </a>
            </h1>

            <table class="entity-table">
                <thead>
                    <tr>
                        <xsl:for-each select="columns/column">
                            <th><xsl:value-of select="@name" /></th>
                        </xsl:for-each>
                    </tr>
                </thead>
                <tbody>
                    <xsl:for-each select="items/item">
                        <tr>
                            <xsl:for-each select="column">
                                <td>
                                    <xsl:apply-templates />
                                </td>
                            </xsl:for-each>
                        </tr>
                    </xsl:for-each>
                </tbody>
            </table>
        </div>
    </xsl:template>

</xsl:stylesheet>

