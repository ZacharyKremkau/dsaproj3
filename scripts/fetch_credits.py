#!/usr/bin/env python3
# scripts/fetch_credits.py

import os
import csv
import re
import time
import requests

# Pull your TMDb key from the environment
TMDB_API_KEY = os.getenv("TMDB_API_KEY", "")
if not TMDB_API_KEY:
    print("❌ Error: TMDB_API_KEY environment variable is not set.")
    exit(1)

BASE_URL = "https://api.themoviedb.org/3"

def parse_title_year(full_title):
    """Split 'Movie Title (YYYY)' into (title, year)."""
    m = re.match(r"(.+?) \((\d{4})\)$", full_title)
    if m:
        return m.group(1), m.group(2)
    return full_title, ""

def tmdb_search(title, year):
    """Return the first TMDb ID matching the title/year, or None."""
    params = {"api_key": TMDB_API_KEY, "query": title}
    if year:
        params["year"] = year
    resp = requests.get(f"{BASE_URL}/search/movie", params=params)
    results = resp.json().get("results", [])
    return results[0]["id"] if results else None

def tmdb_credits(movie_id):
    """Fetch director and top‑5 cast names for a TMDb movie ID."""
    resp = requests.get(f"{BASE_URL}/movie/{movie_id}/credits",
                        params={"api_key": TMDB_API_KEY})
    data = resp.json()
    # find the director
    director = ""
    for member in data.get("crew", []):
        if member.get("job") == "Director":
            director = member.get("name", "")
            break
    # top 5 billed cast
    cast = [c.get("name", "") for c in data.get("cast", [])[:5]]
    return director, cast

def main():
    in_csv  = "data/ml-latest-small/movies.csv"
    out_csv = "data/ml-latest-small/credits.csv"

    with open(in_csv, newline='', encoding='utf-8') as mf, \
            open(out_csv, 'w', newline='', encoding='utf-8') as cf:

        reader = csv.DictReader(mf)
        writer = csv.writer(cf)
        writer.writerow(["movieId","director","actors"])

        for row in reader:
            movieId = row["movieId"]
            title, year = parse_title_year(row["title"])
            tmdb_id = tmdb_search(title, year)

            if not tmdb_id:
                print(f"⚠️  No TMDb match for {title} ({year})")
                continue

            director, actors = tmdb_credits(tmdb_id)
            actor_str = ";".join(actors)
            writer.writerow([movieId, director, actor_str])
            print(f"✔ {movieId}: {title} → {director} / {actor_str}")

            time.sleep(0.05)  # be kind to the API

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print("❌ Error during fetch:", e)
    input("\nDone fetching credits. Press Enter to exit...")
