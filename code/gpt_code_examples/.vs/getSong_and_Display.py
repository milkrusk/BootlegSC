import urequests as requests
from machine import Pin, SPI
import epaper2in13b

# Spotify API access token
ACCESS_TOKEN = "your_access_token"

# E-ink display setup
spi = SPI(1, baudrate=8000000, polarity=0, phase=0)
cs_pin = Pin(5, Pin.OUT)
dc_pin = Pin(2, Pin.OUT)
rst_pin = Pin(0, Pin.OUT)
busy_pin = Pin(4, Pin.IN)
display = epaper2in13b.EPD(spi, cs_pin, dc_pin, rst_pin, busy_pin)

# Spotify API endpoint for currently playing song
SPOTIFY_API_ENDPOINT = "https://api.spotify.com/v1/me/player/currently-playing"

# HTTP headers for Spotify API request
headers = {
    "Authorization": "Bearer " + ACCESS_TOKEN,
    "Content-Type": "application/json"
}

# Make a GET request to the Spotify API to get currently playing song information
response = requests.get(SPOTIFY_API_ENDPOINT, headers=headers)

# Parse response JSON to get the song name and artist name
song_info = response.json()["item"]
song_name = song_info["name"]
artist_name = song_info["artists"][0]["name"]
current_song = "Song: {}\nArtist: {}".format(song_name, artist_name)

# Display the current playing song on the e-ink display
display.init()
display.clear_frame_memory(0xFF)
display.draw_string_at(10, 50, current_song)
display.display_frame()
display.sleep()
