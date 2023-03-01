import urequests as requests
from machine import Pin, SPI
import epaper2in13b
from urandom import getrandbits

# Spotify API access token
ACCESS_TOKEN = "your_access_token"

# E-ink display setup
spi = SPI(1, baudrate=8000000, polarity=0, phase=0)
cs_pin = Pin(5, Pin.OUT)
dc_pin = Pin(2, Pin.OUT)
rst_pin = Pin(0, Pin.OUT)
busy_pin = Pin(4, Pin.IN)
display = epaper2in13b.EPD(spi, cs_pin, dc_pin, rst_pin, busy_pin)

# Button setup
button_pin = Pin(12, Pin.IN, Pin.PULL_UP)

# Spotify API endpoint for currently playing song
SPOTIFY_API_ENDPOINT = "https://api.spotify.com/v1/me/player/currently-playing"

# HTTP headers for Spotify API request
headers = {
    "Authorization": "Bearer " + ACCESS_TOKEN,
    "Content-Type": "application/json"
}

def display_current_song():
    # Make a GET request to the Spotify API to get currently playing song information
    response = requests.get(SPOTIFY_API_ENDPOINT, headers=headers)

    # Parse response JSON to get the song name, artist name, and album art URL
    song_info = response.json()["item"]
    song_name = song_info["name"]
    artist_name = song_info["artists"][0]["name"]
    album_art_url = song_info["album"]["images"][0]["url"]

    # Make a GET request to the album art URL and save the image to a file
    image_data = requests.get(album_art_url).content
    with open("album_art.jpg", "wb") as image_file:
        image_file.write(image_data)

    # Display the current playing song and album art on the e-ink display
    display.init()
    display.clear_frame_memory(0xFF)
    display.draw_string_at(10, 50, "Song: {}\nArtist: {}".format(song_name, artist_name))
    display.draw_bmp("album_art.jpg", 10, 100)
    display.display_frame()
    display.sleep()

def change_to_next_song(pin):
    # Spotify API endpoint for skipping to the next track
    SPOTIFY_API_SKIP_ENDPOINT = "https://api.spotify.com/v1/me/player/next"

    # Make a POST request to the Spotify API to skip to the next track
    response = requests.post(SPOTIFY_API_SKIP_ENDPOINT, headers=headers)

    # Display the new current playing song and album art on the e-ink display
    display_current_song()

# Set up interrupt to trigger the button press event
button_pin.irq(trigger=Pin.IRQ_FALLING, handler=change_to_next_song)

# Display the current playing song and album art on the e-ink display
display_current_song()
