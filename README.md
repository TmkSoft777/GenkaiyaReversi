# GenkaiyaReversi
Genkaiya-chan Reversi on Win32<br>
限界やちゃんのリバーシです。<br>
Win32 といっても WindowsCE 用です。<br>
WindowsNT 用にコンパイルできるかは知りません。<br>

<h2>Version 3.0 beta3<br>
どちらも置けなくなるとメッセージが止まらないバグを修正
旧世代の解像度を間違えて480x360にしてた(正しくは480x320)

<h2>Version 3.0 BETA2</h2>
色バグ修正<br>
完全勝利でメッセージ消えない問題修正<br><br>

<h2>Version 3.0 BETA</h2>
解像度対応<br>
位置調整<br>
commctrl.libを入れたりしてメニュー部分のコンパイルエラーは起きなくなった　が実行時に0が返されて表示されない<br>
テキストがClearTypeに<br><br>

<h2>Version 2.51</h2>
バッファ分割により多少軽量化?<br>
ビットマップを読めなかったときのエラー処理<br>
最大化ボタンが間違って出ているのを修正<br>
board_xとboard_y変数でボードの描画&クリック判定の変更が可<br>
メニューを出そうとしたがなぜかコンパイルエラーが出たのでコメントアウト。<br>
アイコンを地味～に変更<br>
