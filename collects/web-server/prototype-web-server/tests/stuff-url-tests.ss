(module stuff-url-tests mzscheme
  (require (lib "stuff-url.ss" "web-server" "prototype-web-server")
           (planet "test.ss" ("schematics" "schemeunit.plt" 1 1))
           (planet "util.ss" ("schematics" "schemeunit.plt" 1))
           (lib "url.ss" "net")
           (lib "dirs.ss" "setup")
           (lib "file.ss")
           "language-tester.ss")
  
  (require/expose (lib "stuff-url.ss" "web-server" "prototype-web-server")
                  (same-module? url-parts recover-serial))
  
  (provide stuff-url-suite)
  
  (define uri0 (string->url "www.google.com"))
  
  (define (simplify-unsimplify svl pth)
    (let-values ([(l-code simple-mod-map graph fixups sv)
                  (url-parts pth svl)])
      (recover-serial
       pth
       l-code
       simple-mod-map graph fixups sv)))
  
  (define (stuff-unstuff svl uri mod-path)
    (let ([result-uri (stuff-url svl uri mod-path)])
      (unstuff-url result-uri uri mod-path)))
  
  (define the-dispatch
    `(lambda (k*v)
       (lambda (k*v)
         ((car k*v) k*v))))
  
  (define stuff-url-suite
    (make-test-suite
     "Tests for stuff-url.ss"
     
     (make-test-case
      "Test same-module?"
      
      (assert-true
       (same-module? `(file ,(path->string (build-absolute-path (find-collects-dir) "web-server" "prototype-web-server" "abort-resume.ss")))
                     '(lib "abort-resume.ss" "web-server" "prototype-web-server")))
      
      (assert-true
       (same-module? `(file ,(path->string (build-absolute-path (current-directory) "../abort-resume.ss")))
                     '(lib "abort-resume.ss" "web-server" "prototype-web-server")))
      
      (assert-true
       (same-module?
        '(lib "abort-resume.ss" "web-server" "prototype-web-server")
        '(lib "./abort-resume.ss" "web-server" "prototype-web-server"))))
     
     (make-test-case
      "compose url-parts and recover-serial (1)"
      (let-values ([(go ev) (make-eval/mod-path "modules/mm00.ss")])
        (go the-dispatch)
        (let* ([k0 (simplify-unsimplify (ev '(serialize (dispatch-start 'foo)))
                                        `(file "modules/mm00.ss"))]
               [k1 (simplify-unsimplify (ev `(serialize (dispatch (list (deserialize ',k0) 1))))
                                        `(file "modules/mm00.ss"))]
               [k2 (simplify-unsimplify (ev `(serialize (dispatch (list (deserialize ',k1) 2))))
                                        `(file "modules/mm00.ss"))])
          (assert-true (= 6 (ev `(dispatch (list (deserialize ',k2) 3))))))))
     
     (make-test-case
      "compose url-parts and recover-serial (2)"
      (let-values ([(go ev) (make-eval/mod-path "modules/mm01.ss")])
        (go the-dispatch)
        (let* ([k0 (simplify-unsimplify (ev '(serialize (dispatch-start 'foo)))
                                        `(file "modules/mm01.ss"))])
          (assert-true (= 7 (ev `(dispatch (list (deserialize ',k0) 7))))))))
     
     (make-test-case
      "compose stuff-url and unstuff-url and recover the serial"
      (let-values ([(go ev) (make-eval/mod-path "modules/mm00.ss")])
        (go the-dispatch)
        (let* ([k0 (stuff-unstuff (ev '(serialize (dispatch-start 'foo)))
                                  uri0 `(file "modules/mm00.ss"))]
               [k1 (stuff-unstuff (ev `(serialize (dispatch (list (deserialize ',k0) 1))))
                                  uri0 `(file "modules/mm00.ss"))]
               [k2 (stuff-unstuff (ev `(serialize (dispatch (list (deserialize ',k1) 2))))
                                  uri0 `(file "modules/mm00.ss"))])
          (assert-true (= 6 (ev `(dispatch (list (deserialize ',k2) 3)))))))))))