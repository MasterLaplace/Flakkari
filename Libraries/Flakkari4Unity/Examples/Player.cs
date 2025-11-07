using UnityEngine;
using System.Collections.Generic;

using Flk_API = Flakkari4Unity.API;
using CurrentProtocol = Flakkari4Unity.Protocol.V1;

public class Player : Flakkari4Unity.ECS.Entity
{
    public GameObject projectilePrefab;
    [SerializeField] private float velocity = 10;
    [HideInInspector] public Camera playerCamera;
    [HideInInspector] private NetworkClient networkClient = null;
    private bool isLocalPlayer = true;
    public KeyCode forward = KeyCode.W;
    public KeyCode back = KeyCode.S;
    private readonly Dictionary<CurrentProtocol.EventId, CurrentProtocol.EventState> axisEvents = new(4);

    public void Create()
    {
        foreach (CurrentProtocol.EventId eventId in System.Enum.GetValues(typeof(CurrentProtocol.EventId)))
            axisEvents[eventId] = CurrentProtocol.EventState.MAX_STATE;
    }

    public void Destroy()
    {
        if (isLocalPlayer == false)
            networkClient.Send(Flk_API.APIClient.ReqDisconnect());
        GameObject.Destroy(gameObject);
    }

    public void SetupNetworkClient(NetworkClient networkClient)
    {
        this.networkClient = networkClient;
        isLocalPlayer = false;
    }

    public void Update()
    {
        HandleMovement();

        if (isLocalPlayer == true)
            return;

        List<CurrentProtocol.Event> events = new(8);
        Dictionary<CurrentProtocol.EventId, float> axisValues = new(4);

        Net_HandleMovement(ref events, ref axisValues);

        if (events.Count > 0 || axisValues.Count > 0)
            networkClient.Send(Flk_API.APIClient.ReqUserUpdates(events, axisValues));
    }

    private void HandleMovement()
    {
        if (Input.GetKey(this.forward))
            playerCamera.transform.position += playerCamera.transform.forward * velocity * Time.deltaTime;
        if (Input.GetKey(this.back))
            playerCamera.transform.position += -playerCamera.transform.forward * velocity * Time.deltaTime;

        float vertical = Input.GetAxis("Mouse Y");
        if (vertical != 0)
            playerCamera.transform.Rotate(Vector3.right, -vertical);
        float horizontal = Input.GetAxis("Mouse X");
        if (horizontal != 0)
            playerCamera.transform.Rotate(Vector3.forward, -horizontal);

        vertical = Input.GetAxis("Vertical");
        if (vertical != 0)
            playerCamera.transform.Rotate(Vector3.right, -vertical);
        horizontal = Input.GetAxis("Horizontal");
        if (horizontal != 0)
            playerCamera.transform.Rotate(Vector3.forward, -horizontal);
    }

    private void Net_HandleMovement(ref List<CurrentProtocol.Event> events, ref Dictionary<CurrentProtocol.EventId, float> axisValues)
    {
        HandleNetworkInput(this.forward, CurrentProtocol.EventId.MOVE_FRONT, ref events);
        HandleNetworkInput(this.back, CurrentProtocol.EventId.MOVE_BACK, ref events);

        HandleMouseMovement("Mouse X", CurrentProtocol.EventId.LOOK_LEFT, CurrentProtocol.EventId.LOOK_RIGHT, ref axisValues);
        HandleMouseMovement("Mouse Y", CurrentProtocol.EventId.LOOK_DOWN, CurrentProtocol.EventId.LOOK_UP, ref axisValues);
        HandleMouseMovement("Horizontal", CurrentProtocol.EventId.LOOK_LEFT, CurrentProtocol.EventId.LOOK_RIGHT, ref axisValues);
        HandleMouseMovement("Vertical", CurrentProtocol.EventId.LOOK_DOWN, CurrentProtocol.EventId.LOOK_UP, ref axisValues);
    }

    private void HandleNetworkInput(KeyCode keyCode, CurrentProtocol.EventId eventId, ref List<CurrentProtocol.Event> events)
    {
        if (Input.GetKeyDown(keyCode))
        {
            events.Add(new CurrentProtocol.Event { id = eventId, state = CurrentProtocol.EventState.PRESSED });
        }

        else if (Input.GetKeyUp(keyCode))
        {
            events.Add(new CurrentProtocol.Event { id = eventId, state = CurrentProtocol.EventState.RELEASED });
        }
    }

    private void HandleMouseMovement(string axisName, CurrentProtocol.EventId negativeEventId, CurrentProtocol.EventId positiveEventId, ref Dictionary<CurrentProtocol.EventId, float> axisValues)
    {
        float axisValue = Input.GetAxis(axisName);

        if (axisValue != 0)
        {
            axisValues[positiveEventId] = axisValue;
        }
    }
}
