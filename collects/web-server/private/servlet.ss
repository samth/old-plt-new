(module servlet mzscheme
  (require (lib "contract.ss"))
  (require "../managers/manager.ss"
           "../servlet/servlet-structs.ss"
           "connection-manager.ss"
           "../private/request-structs.ss")
  
  (define servlet-prompt (make-continuation-prompt-tag 'servlet))
  (provide servlet-prompt)
  
  (define-struct (exn:fail:servlet:instance exn:fail) ())
  (define-struct servlet (custodian namespace manager handler))
  (define-struct servlet-instance-data (mutex))
  (define-struct execution-context (request))
  
  (define current-servlet (make-parameter #f))
  (define current-servlet-instance-id (make-parameter #f))
  (define current-execution-context (make-parameter #f))
    
  (define (current-servlet-manager)
    (servlet-manager (current-servlet)))
    
  (provide/contract
   [struct (exn:fail:servlet:instance exn:fail)
           ([message string?]
            [continuation-marks continuation-mark-set?])]
   [struct servlet 
           ([custodian custodian?]
            [namespace namespace?]
            [manager manager?]
            [handler (request? . -> . servlet-response?)])]
   [struct servlet-instance-data
           ([mutex semaphore?])]
   [struct execution-context 
           ([request request?])]
   [current-servlet parameter?]
   [current-servlet-instance-id parameter?]
   [current-execution-context parameter?]
   [current-servlet-manager (-> manager?)]))