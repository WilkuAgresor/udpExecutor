String createReply(int retCode)
{
  String reply = "sts";
  reply.concat(separator);
  reply.concat(String(retCode));
  return reply;  
}

String createReply(int retCode, int retVal)
{
  String reply = "rtv";
  reply.concat(separator);
  reply.concat(String(retCode));
  reply.concat(separator);
  reply.concat("val");
  reply.concat(separator);
  reply.concat(String(retVal));
  return reply;  
}

String createSessionReply()
{
  String reply = "sessionId";
  reply.concat(separator);
  reply.concat(String(sessionId));
  reply.concat(separator);
  return reply;
}

#ifdef SPECIAL_PURPOSE_OPENTHERM

#endif
