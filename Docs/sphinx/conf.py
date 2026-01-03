project = "Audio Decimation and FFT Analyzer"
author = "Audio_PDM"

extensions = [
    "sphinx.ext.graphviz",
]

templates_path = []
exclude_patterns = ["_build", "Thumbs.db", ".DS_Store"]

try:
    import sphinx_rtd_theme  # noqa: F401
    html_theme = "sphinx_rtd_theme"
except Exception:
    html_theme = "alabaster"

html_theme_options = {
    "collapse_navigation": False,
    "navigation_depth": 3,
    "style_nav_header_background": "#3b3f46",
}

html_static_path = ["_static"]
html_css_files = ["custom.css"]

graphviz_output_format = "png"
