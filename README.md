# FilterTable

This Qt project shows an implementation of custom filtering in large tables (4 columns, 250000 rows).
Each column has its own QLineEdit. Optionally you can set QCheckBox if you want to filter for `$ Qt::CheckState`.

## Compile

* Load `$ FilterHeader.pro` in your Qt Creator and select Qt5 kit
* Run

If you need Qt4 support, replace
`$ setSectionsClickable()` with `$ setClickable()`
and `$ setSectionsMovable()` with `$ setMovable()`
in `$ filterheader.cpp`.

## Customization

Adjust `$ rowCount` or `$ columnCount` to your needs.

Try playing around with `$ filterReactionTime` if your table is even larger or you have a strong machine.

