from __future__ import annotations

import html
import json
import re
import sys
import time
import urllib.parse
import urllib.request
from html.parser import HTMLParser
from pathlib import Path


BASE_URL = "http://www.darkfallonline.com"
INDEX_URL = f"{BASE_URL}/manual"
OUT_DIR = Path("Docs/References/DarkfallManual")


class ManualParser(HTMLParser):
    def __init__(self) -> None:
        super().__init__(convert_charrefs=False)
        self.links: list[tuple[str, str]] = []
        self.title = ""
        self._in_title = False
        self._title_parts: list[str] = []
        self._div_stack: list[bool] = []
        self._in_body = False
        self._skip_stack: list[str] = []
        self._current_link: str | None = None
        self._current_link_text: list[str] = []
        self.parts: list[str] = []

    def handle_starttag(self, tag: str, attrs: list[tuple[str, str | None]]) -> None:
        attrs_d = {k: v or "" for k, v in attrs}
        if tag == "title":
            self._in_title = True
        if tag in {"script", "style", "noscript"}:
            self._skip_stack.append(tag)
            return
        if tag == "div":
            classes = set(attrs_d.get("class", "").split())
            starts_body = "field-body" in classes
            self._div_stack.append(starts_body)
            if starts_body:
                self._in_body = True
        if tag == "a":
            href = attrs_d.get("href")
            if href:
                self._current_link = href
                self._current_link_text = []
        if self._in_body and tag in {"p", "div", "li", "h1", "h2", "h3", "tr", "br"}:
            self.parts.append("\n")

    def handle_endtag(self, tag: str) -> None:
        if tag == "title":
            self._in_title = False
        if self._skip_stack and self._skip_stack[-1] == tag:
            self._skip_stack.pop()
            return
        if tag == "a" and self._current_link:
            label = normalize_text(" ".join(self._current_link_text))
            self.links.append((self._current_link, label))
            self._current_link = None
            self._current_link_text = []
        if tag == "div" and self._div_stack:
            was_body = self._div_stack.pop()
            if was_body:
                self._in_body = False

    def handle_data(self, data: str) -> None:
        if self._skip_stack:
            return
        text = html.unescape(data)
        if self._in_title:
            self._title_parts.append(text)
        if self._current_link is not None:
            self._current_link_text.append(text)
        if self._in_body:
            self.parts.append(text)

    def handle_entityref(self, name: str) -> None:
        self.handle_data(f"&{name};")

    def handle_charref(self, name: str) -> None:
        self.handle_data(f"&#{name};")

    def close(self) -> None:
        super().close()
        self.title = normalize_text(" ".join(self._title_parts)).replace(" | Darkfall Unholy Wars", "")


def normalize_text(value: str) -> str:
    value = value.replace("\xa0", " ")
    value = re.sub(r"[ \t\r\f\v]+", " ", value)
    value = re.sub(r"\n\s+", "\n", value)
    value = re.sub(r"\n{3,}", "\n\n", value)
    return value.strip()


def fetch(url: str) -> str:
    request = urllib.request.Request(url, headers={"User-Agent": "SpellRiseReferenceCollector/1.0"})
    with urllib.request.urlopen(request, timeout=30) as response:
        return response.read().decode("utf-8", errors="replace")


def parse_page(url: str) -> dict[str, object]:
    parser = ManualParser()
    parser.feed(fetch(url))
    parser.close()
    body = normalize_text("\n".join(parser.parts))
    return {"url": url, "title": parser.title, "text": body, "links": parser.links}


def manual_url(href: str) -> str | None:
    absolute = urllib.parse.urljoin(BASE_URL, href)
    parsed = urllib.parse.urlparse(absolute)
    if parsed.netloc != "www.darkfallonline.com":
        return None
    if not parsed.path.startswith("/manual"):
        return None
    return urllib.parse.urlunparse((parsed.scheme, parsed.netloc, parsed.path, "", "", ""))


def main() -> int:
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    index = parse_page(INDEX_URL)
    urls: list[str] = [INDEX_URL]
    seen = {INDEX_URL}
    for href, _label in index["links"]:  # type: ignore[index]
        url = manual_url(href)
        if url and url not in seen:
            seen.add(url)
            urls.append(url)

    pages = []
    for idx, url in enumerate(urls, 1):
        print(f"[{idx:02d}/{len(urls):02d}] {url}")
        page = parse_page(url)
        pages.append(page)
        time.sleep(0.15)

    (OUT_DIR / "darkfall_manual_pages.json").write_text(
        json.dumps(pages, ensure_ascii=False, indent=2), encoding="utf-8"
    )
    raw = []
    for page in pages:
        raw.append(f"# {page['title']}\n{page['url']}\n\n{page['text']}\n")
    (OUT_DIR / "darkfall_manual_raw.txt").write_text("\n\n".join(raw), encoding="utf-8")
    print(f"Saved {len(pages)} pages to {OUT_DIR}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
