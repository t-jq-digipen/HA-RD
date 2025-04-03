import sys
import requests

BASE_URL = "https://script.google.com/macros/s/AKfycbxUqZIe3FMgZQySC9sEFB1kKp_LFYRRvVWhXgTOTgcFK03ATymomVZPVbWdXHcTp_gxjQ/exec"

def fetch_leaderboard(boss_id):
    """Fetches the leaderboard for the specified boss and saves it to a file."""
    url = f"{BASE_URL}?action=get&boss_id={boss_id}"
    
    try:
        response = requests.get(url)
        
        if response.status_code == 200:
            leaderboard = response.json()  # Parse JSON response
            
            filename = f"leaderboard_{boss_id}.txt"
            with open(filename, "w", encoding="utf-8") as file:
                for entry in leaderboard:
                    file.write(f"{entry['name']} - {entry['score']} - {entry['level_time']}\n")
            
            print(f"Leaderboard fetched and saved to {filename}")
        else:
            print(f"Error fetching leaderboard: {response.status_code}")
    except requests.exceptions.RequestException as e:
        print(f"Request error: {e}")


def add_leaderboard_entry(boss_id, player_name, score, level_time):
    """Adds an entry to the leaderboard for the specified boss."""
    params = {
        'action': 'add',
        'boss_id': boss_id,  # Identify which leaderboard to update
        'name': player_name,
        'score': score,
        'level_time': level_time
    }
    
    try:
        response = requests.post(BASE_URL, data=params)
        
        if response.status_code == 200:
            print(f"Leaderboard for {boss_id} updated successfully.")
        else:
            print(f"Error updating leaderboard: {response.status_code}")
    except requests.exceptions.RequestException as e:
        print(f"Request error: {e}")


if __name__ == "__main__":
    if len(sys.argv) == 2:
        # Fetch leaderboard for the specified boss
        boss_id = sys.argv[1]
        fetch_leaderboard(boss_id)
    
    elif len(sys.argv) == 5:
        # Add leaderboard entry (boss_id, name, score, level_time)
        boss_id = sys.argv[1]
        player_name = sys.argv[2]
        score = int(sys.argv[3])
        level_time = sys.argv[4]
        add_leaderboard_entry(boss_id, player_name, score, level_time)
    
    else:
        print("Invalid arguments.\n")
        print("To fetch leaderboard: python leaderboard_helper.py [boss_id]")
        print("To add entry: python leaderboard_helper.py [boss_id] [name] [score] [level_time]")
