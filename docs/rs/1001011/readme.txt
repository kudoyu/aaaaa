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

デメリットは、画像のダウンロードに時間がかかる場合に、ローディング画像が表示されず白い画面が表示され続けてしまう。
