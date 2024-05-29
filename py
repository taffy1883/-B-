import requests
from bs4 import BeautifulSoup
import json
import sys

def search_bilibili(keyword):
    search_url = f"https://api.bilibili.com/x/web-interface/search/all/v2?keyword={keyword}"
    response = requests.get(search_url)
    data = response.json()

    video_urls = []
    for item in data['data']['result']:
        if item['result_type'] == 'video':
            for video in item['data']:
                video_urls.append({
                    'title': video['title'],
                    'url': video['arcurl'],
                    'bvid': video['bvid']
                })
                if len(video_urls) >= 10:
                    return video_urls
    return video_urls

def get_video_comments(bvid):
    comment_api_url = f"https://api.bilibili.com/x/v2/reply?&jsonp=jsonp&pn=1&type=1&oid={bvid}"
    response = requests.get(comment_api_url)
    comments_data = response.json()
    comments = []

    for comment in comments_data['data']['replies'][:20]:
        comments.append(comment['content']['message'])

    return comments

if __name__ == "__main__":
    keyword = sys.argv[1]
    video_infos = search_bilibili(keyword)

    results = []
    for video_info in video_infos:
        comments = get_video_comments(video_info['bvid'])
        results.append({
            'title': video_info['title'],
            'url': video_info['url'],
            'comments': comments
        })

    with open('bilibili_comments.json', 'w', encoding='utf-8') as f:
        json.dump(results, f, ensure_ascii=False, indent=4)
