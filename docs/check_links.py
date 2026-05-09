#!/usr/bin/env python3
"""Check generated MkDocs links and local assets."""

from html.parser import HTMLParser
from pathlib import Path
from urllib.parse import unquote, urlparse


class LinkParser(HTMLParser):
    """Collects local link and asset references from HTML."""

    def __init__(self):
        super().__init__()
        self.refs = []

    def handle_starttag(self, tag, attrs):
        attrs = dict(attrs)

        if tag in {"a", "link"} and attrs.get("href"):
            self.refs.append(attrs["href"])

        if tag in {"img", "script"} and attrs.get("src"):
            self.refs.append(attrs["src"])


def resolve_candidate(site, html, ref):
    """Resolve a generated reference to a filesystem path."""
    parsed = urlparse(ref)
    path = unquote(parsed.path)

    if path.startswith("/plat-lang/"):
        candidate = site / path.removeprefix("/plat-lang/")
    elif path.startswith("/"):
        candidate = site / path.removeprefix("/")
    else:
        candidate = (html.parent / path).resolve()

    if ref.endswith("/") or not candidate.suffix:
        candidate = candidate / "index.html"

    return candidate


def main():
    """Check all generated HTML files below the site directory."""
    site = Path("site").resolve()
    missing = []

    for html in site.rglob("*.html"):
        parser = LinkParser()
        parser.feed(html.read_text(encoding="utf-8"))

        for ref in parser.refs:
            parsed = urlparse(ref)
            if (
                parsed.scheme
                or parsed.netloc
                or ref.startswith("#")
                or ref.startswith("mailto:")
            ):
                continue

            candidate = resolve_candidate(site, html, ref)
            if not candidate.exists():
                missing.append((html.relative_to(site), ref, candidate))

    if missing:
        for html, ref, candidate in missing:
            print(f"{html}: missing {ref} -> {candidate}")
        return 1

    print("all local docs links/assets resolve")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
