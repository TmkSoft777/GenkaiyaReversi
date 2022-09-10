# GenkaiyaReversi
GenkaiyaReversi<br>
限界やちゃんのリバーシです。<br>
Windows CE, Windows NT どちらにもコンパイルできます。<br>

<h2>Version [BETA] 3.0.2 </h2>
どちらも置けなくなるとメッセージが止まらないバグを修正<br>
旧世代の解像度を間違えて480x360にしてた(正しくは480x320)<br>
旧世代のマスを30pxに変更<br><br>
<h2>Version [BETA] 3.0.1</h2>
色バグ修正<br>
完全勝利でメッセージ消えない問題修正<br><br>

<h2>Version [BETA] 3.0</h2>
解像度対応<br>
位置調整<br>
commctrl.libを入れたりしてメニュー部分のコンパイルエラーは起きなくなった　が実行時に0が返されて表示されない<br>
テキストがClearTypeに<br><br>

<h2>Version [ALPHA] 2.51</h2>
バッファ分割により多少軽量化?<br>
ビットマップを読めなかったときのエラー処理<br>
最大化ボタンが間違って出ているのを修正<br>
board_xとboard_y変数でボードの描画&クリック判定の変更が可<br>
メニューを出そうとしたがなぜかコンパイルエラーが出たのでコメントアウト。<br>
アイコンを地味～に変更<br>
