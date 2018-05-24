package com.simplex.coin2credit

import android.app.Activity
import android.app.PendingIntent
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.graphics.Typeface
import android.nfc.NfcAdapter
import android.nfc.Tag
import android.os.Bundle
import android.support.design.widget.NavigationView
import android.support.v4.content.ContextCompat
import android.support.v4.view.GravityCompat
import android.support.v7.app.ActionBarDrawerToggle
import android.support.v7.app.AppCompatActivity
import android.util.Log
import android.view.Menu
import android.view.MenuItem
import android.view.View
import android.widget.TextView
import android.widget.Toast
import com.github.mikephil.charting.charts.LineChart
import com.github.mikephil.charting.data.Entry
import com.github.mikephil.charting.data.LineData
import com.github.mikephil.charting.data.LineDataSet
import kotlinx.android.synthetic.main.activity_main.*
import kotlinx.android.synthetic.main.app_bar_main.*
import kotlinx.android.synthetic.main.nav_header_main.view.*
import uk.co.chrisjenx.calligraphy.CalligraphyConfig
import uk.co.chrisjenx.calligraphy.CalligraphyContextWrapper
import uk.co.chrisjenx.calligraphy.TypefaceUtils

class MainActivity : AppCompatActivity(), NavigationView.OnNavigationItemSelectedListener, TransactionInterface {
    private var state = 0 //state 0 -> write connected, state 1 -> read amount -> write done
    private lateinit var tag: Tag
    private var message = "Confirmed:David"
    private var error = 0
    private var currentTotal = 0
    private var currentTransaction = 0
    private var read = false

    private lateinit var database: AppDatabase
    private lateinit var databaseValues: List<Transaction>

    override fun databaseRead(transactions: List<Transaction>) {
        databaseValues = transactions
    }

    override fun messageWritten(success: Int) {
        when(success) {
            0 -> {
                Log.d("TAG", "Message Write Failed")
                error++
                if(error < 3) {
                    NdefWriter(this, message).execute(tag)
                } else Log.d("TAG", "Error out")
            }
            1 -> {
                when(state) {
                    0 -> {
                        message = "Done"
                        state = 1
                    }
                    1 -> {
                        message = "Confirmed:David"
                        state = 0
                        read = false

                    }
                }
                Log.d("TAG", "STATE Change " + state)
            }
        }
    }

    override fun relayMessage(data: String) {
        if(!data.contains("Confirmed") && data != "Done" && !read) {
            read = true
            currentTransaction++
            AsyncStore(database).execute(data)
            val lasttransaction = findViewById<TextView>(R.id.lastNum)
            val baltransaction = findViewById<TextView>(R.id.currentBal)
            val nval = data.toInt()
            currentTotal += nval
            val newMessage = String.format("$%d.%d", nval/100, nval%100)
            val balMessage = String.format("$%d.%d", currentTotal/100, currentTotal%100)
            lasttransaction.text = newMessage
            baltransaction.text = balMessage

            NdefWriter(this, message).execute(tag)
            val transactionFragment = TransactionFragment.newInstance(nval)
            transactionFragment.show(supportFragmentManager, "TransactionDialog")
            Log.d("TAG", "State Change " + state + " " + message)
        } else {
            NdefReader(this).execute(tag)
        }

    }

    override fun attachBaseContext(newBase: Context?) {
        super.attachBaseContext(CalligraphyContextWrapper.wrap(newBase))
    }

    private var mNfcAdapter: NfcAdapter? = null
    private val mimeText = "text/plain"

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        CalligraphyConfig.initDefault(CalligraphyConfig.Builder()
                .setDefaultFontPath("fonts/ARCENA.ttf")
                .setFontAttrId(R.attr.fontPath)
                .build()
        )

        setContentView(R.layout.activity_main)
        setSupportActionBar(toolbar)
        error = 0

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

        val tester = charArrayOf('t', 'e', 's', 't', 'i', 'n', 'g')
        database = AppDatabase.getAppDatabase(this, tester)
        AsyncGet(database, this).execute()
        handleIntent(intent)
    }
/*
    override fun onSaveInstanceState(outState: Bundle?) {
        super.onSaveInstanceState(outState)

        outState?.run {
            putInt("State", state)
            putInt("Error", error)
        }
    }

    override fun onRestoreInstanceState(savedInstanceState: Bundle?) {
        super.onRestoreInstanceState(savedInstanceState)

        savedInstanceState?.run {
            if(getInt("Error", 0) < 3) {
                state = getInt("State", 0)
                when (state) {
                    0 -> message = "Done"
                    1 -> message = "Confirmed:David"
                }
            }
        }
    }
*/
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
            R.id.nav_home -> {
                // Handle the camera action
            }
            R.id.nav_recent -> {

            }
            R.id.nav_settings -> {

            }
            R.id.nav_about -> {

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
                tag = intent.getParcelableExtra(NfcAdapter.EXTRA_TAG)
                Log.d("TAG", "Current State: " + state)
                when(state) {
                    0 -> NdefWriter(this, message).execute(tag)
                    1 -> NdefReader(this).execute(tag)
                    else -> Log.d("TAG", "Invalid State")
                }

            } else {
                Log.d("TAG", "Wrong mime: " + type)
            }
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
