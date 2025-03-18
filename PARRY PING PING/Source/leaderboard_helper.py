import requests
import sys

API_URL = "https://script.google.com/macros/s/AKfycbxUqZIe3FMgZQySC9sEFB1kKp_LFYRRvVWhXgTOTgcFK03ATymomVZPVbWdXHcTp_gxjQ/exec"

def fetch_leaderboard():
    response = requests.get(API_URL)
    if response.status_code == 200:
        print(response.json())  # Print leaderboard JSON
    else:
        print("Error fetching leaderboard:", response.status_code)

def submit_score(name, score):
    params = {"name": name, "score": score}
    response = requests.get(API_URL, params=params)
    if response.status_code == 200:
        print("Score submitted successfully!")
    else:
        print("Error submitting score:", response.status_code)

# Handle command-line arguments
if len(sys.argv) > 1:
    action = sys.argv[1]  # "read" or "write"
    if action == "read":
        fetch_leaderboard()
    elif action == "write" and len(sys.argv) == 4:
        submit_score(sys.argv[2], sys.argv[3])
    else:
        print("Usage:")
        print("  python leaderboard_helper.py read")
        print('  python leaderboard_helper.py write "PlayerName" 1000')