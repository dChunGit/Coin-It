package com.simplex.coin2credit

import android.app.Activity
import android.app.PendingIntent
import android.content.Intent
import android.content.IntentFilter
import android.nfc.NfcAdapter
import android.nfc.Tag
import android.os.Bundle
import android.support.design.widget.NavigationView
import android.support.v4.view.GravityCompat
import android.support.v7.app.ActionBarDrawerToggle
import android.support.v7.app.AppCompatActivity
import android.util.Log
import android.view.Menu
import android.view.MenuItem
import android.widget.EditText
import android.widget.TextView
import android.widget.Toast
import kotlinx.android.synthetic.main.activity_main.*
import kotlinx.android.synthetic.main.app_bar_main.*

class MainActivity : AppCompatActivity(), NavigationView.OnNavigationItemSelectedListener, TransactionInterface {
    override fun messageWritten(success: Int) {
        val textView = findViewById<TextView>(R.id.confirm)
        when(success) {
            1 -> textView.text = "Written"
            0 -> textView.text = "Failed"
        }
    }

    override fun relayMessage(message: String) {
        val transactionFragment = TransactionFragment.newInstance(message)
        transactionFragment.show(supportFragmentManager, "TransactionDialog")

    }

    private var mNfcAdapter: NfcAdapter? = null
    private val mimeText = "text/plain"

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        setSupportActionBar(toolbar)
        val editText = findViewById<EditText>(R.id.inputText)

        fab.setOnClickListener { _ ->
            val store = editText.text
            Log.d("DEBUG", store.toString())
            writeIntent(store.toString(), intent)
        }

        val toggle = ActionBarDrawerToggle(
                this, drawer_layout, toolbar, R.string.navigation_drawer_open, R.string.navigation_drawer_close)
        drawer_layout.addDrawerListener(toggle)
        toggle.syncState()

        nav_view.setNavigationItemSelectedListener(this)

        mNfcAdapter = NfcAdapter.getDefaultAdapter(this)
        if(mNfcAdapter == null) {
            Toast.makeText(this, "This device doesn't support NFC", Toast.LENGTH_LONG).show()
            finish()
            return
        } else if(!mNfcAdapter!!.isEnabled) {
            Toast.makeText(this, "NFC disabled", Toast.LENGTH_LONG).show()
        }

        handleIntent(intent)
    }

    override fun onBackPressed() {
        if (drawer_layout.isDrawerOpen(GravityCompat.START)) {
            drawer_layout.closeDrawer(GravityCompat.START)
        } else {
            super.onBackPressed()
        }
    }

    override fun onCreateOptionsMenu(menu: Menu): Boolean {
        // Inflate the menu; this adds items to the action bar if it is present.
        menuInflater.inflate(R.menu.main, menu)
        return true
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        when (item.itemId) {
            R.id.action_settings -> return true
            else -> return super.onOptionsItemSelected(item)
        }
    }

    override fun onNavigationItemSelected(item: MenuItem): Boolean {
        // Handle navigation view item clicks here.
        when (item.itemId) {
            R.id.nav_camera -> {
                // Handle the camera action
            }
            R.id.nav_gallery -> {

            }
            R.id.nav_slideshow -> {

            }
            R.id.nav_manage -> {

            }
            R.id.nav_share -> {

            }
            R.id.nav_send -> {

            }
        }

        drawer_layout.closeDrawer(GravityCompat.START)
        return true
    }

    override fun onNewIntent(intent: Intent) {
        handleIntent(intent)
    }

    private fun handleIntent(intent: Intent) {
        val action = intent.action
        if(NfcAdapter.ACTION_NDEF_DISCOVERED == action) {
            val type = intent.type
            if(mimeText == type) {
                val tag = intent.getParcelableExtra<Tag>(NfcAdapter.EXTRA_TAG)
                NdefReader(this).execute(tag)
                //NdefWriter(this, "Confirmed").execute(tag)

            } else {
                Log.d("TAG", "Wrong mime: " + type)
            }
        }
    }

    private fun writeIntent(payload: String, intent: Intent) {
        val action = intent.action
        if(NfcAdapter.ACTION_NDEF_DISCOVERED == action) {
            writeMessageToTag(payload, intent)
        }
    }

    private fun writeMessageToTag(payload: String, intent: Intent?) {
        Log.d("Debug", "Write to Tag")
        intent?.let {
            val tag = intent.getParcelableExtra<Tag>(NfcAdapter.EXTRA_TAG)
            NdefWriter(this, payload).execute(tag)
        }
    }

    override fun onResume() {
        super.onResume()

        setupForegroundDispatch(this, mNfcAdapter)
    }

    override fun onPause() {
        super.onPause()

        stopForegroundDispatch(this, mNfcAdapter)
    }

    private fun setupForegroundDispatch(activity: Activity, adapter: NfcAdapter?) {
        val intent = Intent(activity.applicationContext, activity.javaClass)
        intent.flags = Intent.FLAG_ACTIVITY_SINGLE_TOP

        val pendingIntent = PendingIntent.getActivity(activity.applicationContext, 0, intent, 0)

        val filters = arrayOfNulls<IntentFilter>(1)
        val techList = arrayOf<Array<String>>()

        // Notice that this is the same filter as in our manifest.
        filters[0] = IntentFilter()
        filters[0]!!.addAction(NfcAdapter.ACTION_NDEF_DISCOVERED)
        filters[0]!!.addCategory(Intent.CATEGORY_DEFAULT)
        try {
            filters[0]!!.addDataType(mimeText)
        } catch (e: IntentFilter.MalformedMimeTypeException) {
            throw RuntimeException("Check your mime type.")
        }

        adapter!!.enableForegroundDispatch(activity, pendingIntent, filters, techList)
    }

    private fun stopForegroundDispatch(activity: Activity, adapter: NfcAdapter?) {
        adapter!!.disableForegroundDispatch(activity)
    }
}
