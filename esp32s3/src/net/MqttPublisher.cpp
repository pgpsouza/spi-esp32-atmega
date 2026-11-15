#include "MqttPublisher.h"

MqttPublisher::MqttPublisher(Client &networkClient, const char *host, uint16_t port, const char *clientId,
                             const char *user, const char *password, uint32_t retryIntervalMs)
    : client(networkClient), host(host), port(port), clientId(clientId),
      user(user), password(password), retryIntervalMs(retryIntervalMs)
{
}

void MqttPublisher::begin()
{
  client.setServer(host, port);
}

bool MqttPublisher::maintain()
{
  if (!client.connected())
    reconnect_if_due();
  client.loop();
  return client.connected();
}

bool MqttPublisher::publish(const char *topic, const char *payload)
{
  return client.publish(topic, payload);
}

void MqttPublisher::reconnect_if_due()
{
  if (hasAttempted && millis() - lastAttemptMs < retryIntervalMs)
    return;
  hasAttempted = true;
  lastAttemptMs = millis();
  const char *authUser = user[0] == '\0' ? nullptr : user;
  client.connect(clientId, authUser, password);
}
