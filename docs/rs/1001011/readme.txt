修正前
https://kudoyu.github.io/aaaaa/rs/1001011/recommend1.html

修正後
https://kudoyu.github.io/aaaaa/rs/1001011/recommend3.html

差分は、
カルーセル1ページ目の8枚ぐらいの画像について、
* js-imgLoading をはずす
* <img data-src を <img src にする
の2点。

ファーストビューの画像が表示されるまでが、修正前のほうがかなり速い。ただし DOMContentLoad や Load イベントは少し遅くなる。

なんで速いかというと、
修正前は、
css と js のダウンロード完了後に js が実行されて lazy load が発火してから画像のダウンロードが開始される。
それに対して修正後は、
カルーセル1ページ目の画像は速いタイミング（jsやcssのダウンロードと同時）にダウンロードが開始されるため、1ページ目の画像が表示されるまでが速い。

画像のダウンロードが開始するのが 300ms ぐらい違う。

https://developers.google.com/speed/pagespeed/insights/
の点数もかなりよくなる。

課題は、
1. 画像のダウンロードに時間がかかる場合に、ローディング画像が表示されず白い画面が表示され続けてしまう。
2. 画面サイズにより1ページの枚数が異なるため、何枚を非lazyにするかを制御したい。

1.に対処するため
https://kudoyu.github.io/aaaaa/rs/1001011/recommend4.html
という案はどうか。 <img> の onload で owlLazyloadCallback と同等の処理を行う。

2.は簡単ではなさそう。
画面サイズ（カルーセル要素の横幅）を取得するためにcssの読み込みを待つ必要がおそらくあり、画像のダウンロード開始が遅れる。
あるいは、「n番目の画像タグ」をjsで操作するのはDOMツリー構築（DOMContentLoaded）を待ってからが無難でそれにはcssとjsを待つ必要がある。
（自信無し。識者の意見求ム）
非lazyにする枚数＝想定する最小の画面サイズでの枚数、という設定で許してほしい。
