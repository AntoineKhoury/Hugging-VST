import websocket
import json
import logging
import requests
import sys
import subprocess
logging.basicConfig(level=logging.DEBUG)

# Check if at least one command-line argument is provided
if len(sys.argv) < 2:
    print("Usage: Get_audio.py <your_argument>")
    sys.exit(1)

# Access the argument passed from the command line
your_argument = sys.argv[1]
print(your_argument)

# Define the on_message function to handle incoming messages
def on_message(ws, message):
    print(f"Received: {message}")
    
    # Parse the received message as JSON
    msg_data = json.loads(message)
    
    # Check the content and respond accordingly
    if msg_data.get("msg") == "send_hash":
        response = {
            "fn_index": 0,
            "session_hash": "5qsq95343ei"
        }
        ws.send(json.dumps(response))
    
    elif msg_data.get("msg") == "send_data":
        response = {
            "data": [your_argument, None],
            "event_data": None,
            "fn_index": 0,
            "session_hash": "5qsq95343ei"
        }
        ws.send(json.dumps(response))

    # Check if the message is of type "process_completed"
    elif msg_data.get("msg") == "process_completed":
        # Extract the file path
        file_path = msg_data["output"]["data"][0][0]["name"]
        
        # Construct the download URL
        download_url = f"https://facebook-musicgen.hf.space/file={file_path}"
        
        # Download the file
        response = requests.get(download_url)
        if response.status_code == 200:
            with open('output.mp4', 'wb') as file:
                file.write(response.content)
            print("File downloaded successfully!")
        else:
            print(f"Failed to download the file. Status code: {response.status_code}")

    

# Define the on_error function to handle any errors
def on_error(ws, error):
    print(f"Error: {error}")

# Define the on_close function to handle when the connection is closed
def on_close(ws, close_status_code, close_msg):
    print("Connection closed")

# Define the on_open function to handle when the connection is opened
def on_open(ws):
    print("Connection opened")
    
    # If there's specific data or commands you need to send to the server upon connecting, 
    # you would do it here. For example:
    # ws.send('{"action": "start_inference", "model": "facebook-musicgen"}')

# Create the WebSocket connection
ws = websocket.WebSocketApp("wss://facebook-musicgen.hf.space/queue/join",
                            on_message=on_message,
                            on_error=on_error,
                            on_close=on_close)
ws.on_open = on_open
ws.run_forever()

