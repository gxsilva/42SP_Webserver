#ifndef HTTPSTATUSCODE_HPP
#define HTTPSTATUSCODE_HPP

enum    HttpStatusCode
{
    /*1xx informational response

        Como o padrão HTTP/1.0 não definiu nenhum código de status 1xx, 
        os servidores não devem enviar uma resposta 1xx a um cliente compatível com HTTP/1.0 
        exceto em condições experimentais.
    */

    /* **************************** */

    /*2xx success

        O servidor recebeu seu pedido, entendeu o que você queria e processou a ação com sucesso. 
        É o sinal verde de que a comunicação funcionou.
    */
    OK                          = 200, //O pedido padrão de sucesso. A resposta contém o que você pediu.
    CREATED                     = 201, //O pedido funcionou e resultou na criação de um novo recurso (comum em métodos POST).
    ACCEPTED                    = 202, //O servidor aceitou o pedido, mas ele ainda está sendo processado (comum em tarefas em segundo plano).
    NO_CONTENT                  = 204, //Sucesso total, mas o servidor não tem nada para te enviar de volta no corpo da mensagem.

    /* **************************** */

    /*3xx redirection

        O recurso que você pediu não está exatamente onde você clicou. 
        O navegador geralmente segue o novo caminho sozinho (se for um GET), 
        mas o código serve para avisar que houve um desvio no trajeto.
    */
    MOVED_PERMANENTLY            = 301, //O endereço mudou para sempre. O navegador deve usar a nova URL daqui para frente.
    FOUND                        = 302, //O recurso está em outro lugar agora, mas é temporário. Continue usando a URL original no futuro.
    NOT_MODIFIED                 = 304, //O recurso não mudou desde a última vez que você o baixou. Use o que está no seu cache para economizar banda.

   /* **************************** */

   /*4xx client error  
    
        Houve um erro na requisição enviada. Pode ser um endereço errado,
        falta de permissão ou dados inválidos. O servidor te devolve o 
        erro para você corrigir o que enviou.
    */
    BAD_REQUEST                  = 400, //A requisição está malformada. O servidor não conseguiu entender a sintaxe.
    FORBIDDEN                    = 403, //O servidor entendeu quem você é, mas você não tem permissão para acessar esse recurso específico.
    NOT_FOUND                    = 404, //O servidor não encontrou nada no endereço que você passou.
    CONTENT_TOO_LARGE            = 413, //O arquivo ou corpo da requisição que você tentou enviar é grande demais para o servidor.
    URI_TOO_LONG                 = 414, //O link (URL) enviado é longo demais para o servidor processar.
    TOO_MANY_REQUESTS            = 429, //Você está enviando requisições rápido demais. O servidor está te bloqueando temporariamente (Rate Limit).

   /* **************************** */

   /*5xx server error
   
        O servidor sabe que você pediu algo certo, mas ele falhou 
        internamente. Ele encontrou um bug ou está sobrecarregado 
        e não consegue completar a tarefa agora.
   */
    INTERNAL_SERVER_ERROR       = 500, //O "erro genérico". Algo quebrou no código do servidor e ele não sabe explicar exatamente o quê.
    NOT_IMPLEMENTED             = 501, //O servidor não suporta a funcionalidade necessária para completar a requisição.
    //BAD_GATEWAY                 = 502, //O servidor, agindo como um intermediário, recebeu uma resposta inválida de outro servidor acima dele.
    SERVICE_UNAVAILABLE         = 503, //O servidor está sobrecarregado ou em manutenção. Tente novamente mais tarde.
    GATEWAY_TIMEOUT             = 504, //O servidor demorou demais para responder. O intermediário desistiu de esperar.
    HTTP_VERSION_NOT_SUPPORTED  = 505  //O servidor não suporta a versão do protocolo HTTP que você usou na requisição.
    
};

#endif