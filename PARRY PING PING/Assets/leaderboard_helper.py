import sys
import requests

def fetch_leaderboard():
    url = 'https://script.google.com/macros/s/AKfycbxUqZIe3FMgZQySC9sEFB1kKp_LFYRRvVWhXgTOTgcFK03ATymomVZPVbWdXHcTp_gxjQ/exec?action=get'
    
    try:
        response = requests.get(url)
        
        if response.status_code == 200:
            leaderboard = response.json()  # Returns leaderboard data as JSON
            
            # Write to leaderboard.txt
            with open("leaderboard.txt", "w", encoding="utf-8") as file:
                for entry in leaderboard:
                    file.write(f"{entry['name']} - {entry['score']} - {entry['level_time']}\n")
            
            print("Leaderboard fetched and saved to leaderboard.txt")
        else:
            print(f"Error fetching leaderboard: {response.status_code}")
    except requests.exceptions.RequestException as e:
        print(f"Request error: {e}")


def add_leaderboard_entry(player_name, score, level_time):
    url = 'https://script.google.com/macros/s/AKfycbxUqZIe3FMgZQySC9sEFB1kKp_LFYRRvVWhXgTOTgcFK03ATymomVZPVbWdXHcTp_gxjQ/exec'
    
    params = {
        'action': 'add',
        'name': player_name,
        'score': score,
        'level_time': level_time  # Add the level_time parameter
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
    elif len(sys.argv) == 4:
        # Command-line arguments: add leaderboard entry
        player_name = sys.argv[1]
        score = int(sys.argv[2])
        level_time = sys.argv[3]  # Parse the level_time argument
        add_leaderboard_entry(player_name, score, level_time)
    else:
        print("Invalid arguments. Use: python leaderboard_helper.py [name] [score] [level_time]")
