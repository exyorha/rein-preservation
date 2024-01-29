from mitmproxy import contentviews, flow, http
from mitmproxy.addonmanager import Loader
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.primitives import padding
import struct

def request(context, flow = None):
    if flow:
        if flow.client_conn.ssl_established:
            # TLS SNI or Host header
            flow.request.host = flow.client_conn.connection.get_servername() or flow.request.pretty_host(hostheader=True)

            # If you use a https2http location as default destination, these attributes need to be corrected as well:
            flow.request.port = 443
            flow.request.scheme = "https"
        else:
            # Host header
            flow.request.host = flow.request.pretty_host(hostheader=True)

def decrypt_api_request(data):

    key = b'1234567890ABCDEF'
    iv = b'it8bAjktKdFIBYtU'

    cipher = Cipher(algorithms.AES(key), modes.CBC(iv))
    padder = padding.PKCS7(algorithms.AES.block_size)

    decrypted_data = bytes()

    while data:
        msg_is_compressed, length = struct.unpack("!?i", data[:5])
        decoded_message = struct.unpack("!%is" % length, data[5 : 5 + length])[0]

        decryptor = cipher.decryptor()
        padder_instance = padder.unpadder()
        decrypted_message = padder_instance.update(decryptor.update(decoded_message))
        decrypted_message += padder_instance.update(decryptor.finalize())
        decrypted_message += padder_instance.finalize()

        decrypted_data += struct.pack("!?i", msg_is_compressed, len(decrypted_message))
        decrypted_data += decrypted_message

        data = data[5 + length :]

    return decrypted_data

class EncryptedGrpcView(contentviews.grpc.ViewGrpcProtobuf):
    name = 'encrypted-grpc'

    def __call__(
        self,
        data: bytes,
        *,
        content_type: str | None = None,
        flow: flow.Flow | None = None,
        http_message: http.Message | None = None,
        **unknown_metadata,
    ) -> contentviews.TViewResult:

        data = decrypt_api_request(data)

        unused_title, contents = contentviews.grpc.ViewGrpcProtobuf.__call__(self, data,
                                                                        content_type = content_type,
                                                                        flow = flow,
                                                                        http_message = http_message,
                                                                        unknown_metadata = unknown_metadata)

        return "Encrypted gRPC", contents

    def render_priority(
        self,
        data: bytes,
        *,
        content_type: str | None = None,
        flow: flow.Flow | None = None,
        http_message: http.Message | None = None,
        **unknown_metadata,
    ) -> float:
        if content_type == "application/grpc" and flow.request.pretty_host == "api.app.nierreincarnation.com":
            return 10
        else:
            return 0

view = EncryptedGrpcView()


def load(loader: Loader):
    contentviews.add(view)


def done():
    contentviews.remove(view)
