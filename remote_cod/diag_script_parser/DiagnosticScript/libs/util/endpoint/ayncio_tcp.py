import asyncio

client_tcp = None

class TcpClient(asyncio.Protocol):
	message = 'Testing'

	def connection_made(self, transport):
		self.transport = transport
		self.transport.write(self.message.encode())
		print('data sent: {}'.format(self.message))

	def data_received(self, data):
		self.data = format(data.decode())
		print('data received: {}'.format(data.decode()))

	def send_data_to_tcp(self, data):
		self.transport.write(data.encode())

	def connection_lost(self, exc):
		msg = 'Connection lost with the server...'
		info = self.transport.get_extra_info('peername')

@asyncio.coroutine
def do_connect():
	global client_tcp
	while True:
		try:
			loop = asyncio.get_event_loop()
			client_tcp = yield from loop.create_connection(TcpClient, 'localhost', 8000)
		except OSError:
			print("Server not up retrying in 5 seconds...")
			yield from asyncio.sleep(1)
		else:
			break

def setUp():
	loop = asyncio.get_event_loop()
	loop.run_until_complete(do_connect())
	# todo start a now thread
	loop.run_forever()
