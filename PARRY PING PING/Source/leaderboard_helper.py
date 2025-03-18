import sys
import requests

def fetch_leaderboard():
    url = 'https://script.google.com/macros/s/AKfycbxUqZIe3FMgZQySC9sEFB1kKp_LFYRRvVWhXgTOTgcFK03ATymomVZPVbWdXHcTp_gxjQ/exec?action=get'
    
    try:
        response = requests.get(url)
        
        if response.status_code == 200:
            leaderboard = response.json()  # Returns leaderboard data as JSON
            for entry in leaderboard:
                print(f"{entry['name']} - {entry['score']}")
        else:
            print(f"Error fetching leaderboard: {response.status_code}")
    except requests.exceptions.RequestException as e:
        print(f"Request error: {e}")


def add_leaderboard_entry(player_name, score):
    url = 'https://script.google.com/macros/s/AKfycbxUqZIe3FMgZQySC9sEFB1kKp_LFYRRvVWhXgTOTgcFK03ATymomVZPVbWdXHcTp_gxjQ/exec'
    
    params = {
        'action': 'add',
        'name': player_name,
        'score': score
    }
    
    try:
        response = requests.post(url, data=params)
        
        if response.status_code == 200:
            print("Leaderboard updated successfully.")
        else:
            print(f"Error updating leaderboard: {response.status_code}")
    except requests.exceptions.RequestException as e:
        print(f"Request error: {e}")


if __name__ == "__main__":
    if len(sys.argv) == 1:
        # No command-line arguments, fetch leaderboard
        fetch_leaderboard()
    elif len(sys.argv) == 3:
        # Command-line arguments: add leaderboard entry
        player_name = sys.argv[1]
        score = int(sys.argv[2])
        add_leaderboard_entry(player_name, score)
    else:
        print("Invalid arguments. Use: python leaderboard_helper.py [name] [score]")

