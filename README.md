# updown
- AviUtl拡張編集の設定ダイアログ上のUPDOWNボタン(<>)をクリックした時の変動値を設定可能にする
- Shiftキーを押しながらクリックした時の変動値を個別に設定可能にする
### 動作環境
拡張編集バージョン0.92のみ対応
### 設定方法
- updown.iniを編集します
- [Default]が通常クリック時、[Shift]がShift+クリック時の値です
- scale_0が整数、scale_0.0が小数点第1、scale_0.00が小数点第2まである項目での値です
- 変動値を設定した数だけ乗算します
> 例: 0=1, 0.0=10, 0.00=100 にすると最小単位ずつではなく、共通して1.00だけ変動するようになります
- 仕様として、長押しによって大きく変動する場合にも同じだけ乗算されます
