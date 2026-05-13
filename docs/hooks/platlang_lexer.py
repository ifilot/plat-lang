"""Pygments lexer registration for plat-lang documentation."""

from pygments.lexer import RegexLexer, words
from pygments.lexers import LEXERS, _lexer_cache
from pygments.token import Comment, Keyword, Name, Number, Operator, Punctuation, String, Text


class PlatlangLexer(RegexLexer):
    """Syntax highlighter for the current plat-lang language surface."""

    name = "plat-lang"
    aliases = ["platlang", "plat"]
    filenames = ["*.plat"]
    mimetypes = ["text/x-platlang"]

    tokens = {
        "root": [
            (r"\s+", Text),
            (r"#.*?$", Comment.Single),
            (r'"(?:[^\\\"\n]|\\[nt\"\\])*"', String.Double),
            (r"\b[0-9]+(?:\.[0-9]+)?\b", Number.Float),
            (
                words(
                    (
                        "loat",
                        "funksie",
                        "es",
                        "angesj",
                        "zolang",
                        "veur",
                        "enj",
                        "trok",
                        "aafbraeke",
                        "euversjlaon",
                    ),
                    prefix=r"\b",
                    suffix=r"\b",
                ),
                Keyword,
            ),
            (words(("en", "of"), prefix=r"\b", suffix=r"\b"), Operator.Word),
            (words(("niks", "woar", "kwatsj"), prefix=r"\b", suffix=r"\b"), Keyword.Constant),
            (
                words(("nommer", "nómmer", "teks", "tabel"), prefix=r"\b", suffix=r"\b"),
                Keyword.Type,
            ),
            (words(("aafdrokke", "aafdrökke", "inveure"), prefix=r"\b", suffix=r"\b"), Name.Builtin),
            (r"==|!=|<=|>=|[+\-*/%<>=!]", Operator),
            (r"[(){}\[\]:,.]", Punctuation),
            (r"[^\W\d]\w*", Name),
        ],
    }


def register_platlang_lexer() -> None:
    """Register plat-lang aliases with Pygments for Markdown code fences."""

    LEXERS["PlatlangLexer"] = (
        __name__,
        PlatlangLexer.name,
        tuple(PlatlangLexer.aliases),
        tuple(PlatlangLexer.filenames),
        tuple(PlatlangLexer.mimetypes),
    )
    _lexer_cache[PlatlangLexer.name] = PlatlangLexer


def on_startup(command, dirty):
    register_platlang_lexer()


register_platlang_lexer()
