using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class AppWifi : MonoBehaviour
{
    [SerializeField] private TextMeshProUGUI _textData;

    
    
    private static AppWifi instance;
    private Thread receiveThread;
    private UdpClient _dataReceiveClient;
    private IPEndPoint _receiveEndPointData;
    public string _ipData = "192.168.0.108";

    public int _receivePortData = 3100;
    public int _sendPortData = 44444;


    private bool isInitialized;
    private Queue receiveQueue;

    private int _dataReceived;

    private float time = 0;
    
    private void Awake()
    {
        Initialize();
    }
    
    private void Initialize()
    {
        instance = this;
        _receiveEndPointData = new IPEndPoint(IPAddress.Parse(_ipData), _sendPortData);
        _dataReceiveClient = new UdpClient(_receivePortData);
        receiveQueue = Queue.Synchronized(new Queue());


        
        receiveThread = new Thread(new ThreadStart(ReceiveDataListener));
        receiveThread.IsBackground = true;
        receiveThread.Start();
        isInitialized = true;
    }

    private void ReceiveDataListener()
    {
        while (true)
        {
            try
            {
                byte[] dataPulse = _dataReceiveClient.Receive(ref _receiveEndPointData);
                receiveQueue.Enqueue(dataPulse);
            }
            catch (System.Exception ex)
            {
                Debug.Log(ex.ToString());
            }
        }
    }
    private void OnDestroy()
    {
        TryKillThread();
    }
    private void OnApplicationQuit()
    {
        TryKillThread();
    }
    private void TryKillThread()
    {
        if (isInitialized)
        {
            receiveThread.Abort();
            receiveThread = null;

            _dataReceiveClient.Close();
            _dataReceiveClient = null;

            Debug.Log("Thread killed");
            isInitialized = false;
        }
    }
    private void sendStringData(string message)
    {
        try
        {
            byte[] data = Encoding.UTF8.GetBytes(message);
            _dataReceiveClient.Send(data, data.Length, _receiveEndPointData);
        }
        catch (System.Exception err)
        {
            print(err.ToString());
        }
    }
    void Update()
    {
        time += Time.deltaTime;
        if (time >= 1)
        {
            buttonTest();
            Debug.Log("hola1");
            time = 0;
        }
        if (receiveQueue.Count != 0)
        {
            byte[] message = (byte[])receiveQueue.Dequeue();
            if (message == null)
                return;
            Debug.Log("Mensaje de llegada");
            _dataReceived = Convert.ToInt32(message[0]);
            Debug.Log(_dataReceived.ToString());
            _textData.text = _dataReceived.ToString();
        }
    }
    public void buttonTest()
    {
        sendStringData("1");
        Debug.Log("hola12");
    }
}
